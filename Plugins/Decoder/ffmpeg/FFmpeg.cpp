#include "FFmpeg.h"

#include <QFile>
#include <QDebug>

#include "MediaResource.h"
//extern "C"{
//#include <libavformat/avformat.h>
//#include <libavcodec/avcodec.h>
//#include <libavutil/dict.h>
//#include <libavutil/avutil.h>
//#include <libavutil/mathematics.h>
//}

//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1: 55.39.100
//extern "C"{
//#include <libavutil/channel_layout.h>
//#include <libavutil/frame.h>
//}
//#endif

#include "AudioMetaGroupObject.h"

namespace PhoenixPlayer {
namespace Decoder {
namespace FFmpegDecoder {

using namespace PhoenixPlayer;

FFmpeg::FFmpeg(QObject *parent)
    : IDecoder(parent)
    , m_audioFormat(AudioParameters::PCM_UNKNOWM)
//    , m_parameter(AudioParameters())
    , container(nullptr)
    , ctx(nullptr)
    , frame(nullptr)
    , audio_st(nullptr)
    , m_resource(nullptr)
    , current_in_seconds(0)
    , duration_in_seconds(0)
    , m_output_at(0)
    , m_bitrate(0)
{
}

FFmpeg::~FFmpeg()
{
    close ();
}

bool FFmpeg::initialize(MediaResource *res)
{
    if (!res) {
        qCritical()<<"No media resource !!";
        return false;
    }
    m_resource = res;

    qDebug()<<">>>>>>>>>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<";
    if (container) {
        close ();
    }

    QString file = m_resource->getUri ();

    if (avformat_open_input (&container, file.toUtf8 ().constData (), dumpSourceFormat (file), NULL) < 0) {
        qWarning()<<Q_FUNC_INFO<<"can't open file "<<file;
        return false;
    }

    if(avformat_find_stream_info (container, NULL) < 0){
        qWarning()<<Q_FUNC_INFO<<"Stream info load failed";
        return false;
    }
    if (container->pb) {
        container->pb->eof_reached = 0;
    }
    container->flags |= AVFMT_FLAG_GENPTS;

    av_read_play (container);

    //find audio stream id;
    audio_stream_id = -1;
    for(unsigned int i=0; i<container->nb_streams; ++i){
        if(container->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_stream_id = i;
            break;
        }
    }
    if(audio_stream_id == -1){
        qWarning()<<Q_FUNC_INFO<<"No audio stream";
        close ();
        return false;
    }
    qDebug()<<"Stream Id "<<audio_stream_id;

    //set codec context by audio_stream_id
    //then set codec and audio parameter
    ctx = container->streams[audio_stream_id]->codec;
    AVCodec *codec = avcodec_find_decoder(ctx->codec_id);
    if(!codec){
        qWarning()<<Q_FUNC_INFO<<"Cannot find codec";
        close ();
        return false;
    }
    if (ctx->channels == 1) {
        ctx->request_channel_layout = AV_CH_LAYOUT_MONO;
        m_parameter.setChannels (ctx->channels);
    } else {
        ctx->request_channel_layout = AV_CH_LAYOUT_STEREO;
        m_parameter.setChannels (2);
    }
    m_parameter.setSampleRate (ctx->sample_rate);

    qDebug()<<" codec ["<<codec->long_name<<"]";
    qDebug()<<" AudioParameters ["<<m_parameter.parametersInfo ()<<"]";

    if(avcodec_open2 (ctx, codec, nullptr) < 0){
        qWarning()<<Q_FUNC_INFO<<"Codec cannot be opened";
        close ();
        return false;
    }
    //dump values
    av_dump_format (container, 0, 0, 0);

    audio_st = container->streams[audio_stream_id];
    duration_in_seconds = container->duration / AV_TIME_BASE;
    if (ctx->codec_id == AV_CODEC_ID_SHORTEN) { //ffmpeg bug workaround
        duration_in_seconds = 0;
    }
    sfmt = ctx->sample_fmt;
    switch (sfmt) {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
        m_audioFormat = AudioParameters::PCM_S8;
        break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
        m_audioFormat = AudioParameters::PCM_S16LE;
        break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
        m_audioFormat = AudioParameters::PCM_S32LE;
        break;
    default:
        qWarning()<<"unsupported audio format";
        return false;
    }
    m_parameter.setFormat (m_audioFormat);
    if (container->bit_rate) {
//        qWarning()<<"no AVFormatContext bit rate found";
        m_bitrate = container->bit_rate;
    }
    if (ctx->bit_rate) {
//        qWarning()<<"no AVCodecContext bit rate found";
        m_bitrate = ctx->bit_rate;
    }
    if (container->bit_rate != ctx->bit_rate) {
        qDebug()<<QString("AVFormatContext bit rate [%1], AVCodecContext bit rate [%2]")
                  .arg (container->bit_rate).arg (ctx->bit_rate);
    }
    //    frameFinished=0;
    //    packetFinished=0;
    //    vpbuffer_write=0;
    //    temp_write=0;
    //    remainder_read=0;
    //    remainder_counter=0;

    if (!frame)
        frame = av_frame_alloc();

    current_in_seconds = 0;
    qDebug()<<">>>>>>>>>>> "<<Q_FUNC_INFO<<" opend <<<<<<<<<";

    return true;
}

quint64 FFmpeg::durationInSeconds()
{
    return duration_in_seconds;
}

void FFmpeg::setPosition(qreal pos)
{

}

int FFmpeg::bitrate()
{
    return m_bitrate/1000;
}

qint64 FFmpeg::runDecode(char *data, qint64 maxSize)
{
    qDebug()<<" >>>> "<<Q_FUNC_INFO<<" <<<<";
    if (!m_output_at)
        fillBuffer ();

    if(!m_output_at) {
        qCritical()<<Q_FUNC_INFO<<" No buffer out put index find";
        return 0;
    }
    qint64 len = qMin(m_output_at, maxSize);
    qDebug()<<"try to copy buffer size "<<len;

    if (av_sample_fmt_is_planar (ctx->sample_fmt) && m_parameter.channels () > 1) {
        qDebug()<<" try to decode planar frame ";
        int bps = av_get_bytes_per_sample (ctx->sample_fmt);
        qDebug()<<" bps "<<bps;
        for (int i=0; i<(len>>1); i+=bps) {
            memcpy (data+2*i, frame->extended_data[0]+i, bps);
            memcpy (data+2*i+bps, frame->extended_data[1]+i, bps);
        }
        m_output_at -= len;
        qDebug()<<" in planar frame, now out put at "<<m_output_at;
        memmove (frame->extended_data[0], frame->extended_data[0]+len/2, m_output_at/2);
        memmove (frame->extended_data[1], frame->extended_data[1]+len/2, m_output_at/2);
    } else {
        memcpy (data, frame->extended_data[0], len);
        m_output_at -= len;
        qDebug()<<" no planar frame, now out put at "<<m_output_at;
        memmove (frame->extended_data[0], frame->extended_data[0]+len, m_output_at);
    }
    if (ctx->sample_fmt == AV_SAMPLE_FMT_FLTP || ctx->sample_fmt == AV_SAMPLE_FMT_FLT) {
        qDebug()<<" convert float to signed 32 bit LE ";
        for (int i=0; i<(len>>2); ++i) {
            int32_t *out = (int32_t*)data;
            float *in = (float*)data;
            out[i] = qBound(-1.0f, in[i], +1.0f) * (double) 0x7fffffff;
        }
    }
    qDebug()<<" >>>> "<<Q_FUNC_INFO<<" finish <<<<";
    return len;
}

AudioParameters FFmpeg::audioParameters() const
{
    return m_parameter;
}

bool FFmpeg::close()
{
    if (ctx)
        avcodec_close(ctx);
    ctx = nullptr;

    if (container) {
        avformat_close_input(&container);
        avformat_free_context(container);
    }
    container = nullptr;

    if (frame) {
        av_frame_free(&frame);
    }
    frame = nullptr;

    return true;
}

void FFmpeg::fillBuffer()
{
    qDebug()<<" >>>>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<<< ";

    while (!m_output_at) {
        qDebug()<<"loop , current tmpPacket.size "<<m_tmpPacket.size;
        if (!m_tmpPacket.size) {
            int packetFinishRet = av_read_frame(container, &packet);
            if (packetFinishRet < 0) {
                qCritical()<<Q_FUNC_INFO<<"read frame of a stream error, error code="<<packetFinishRet;
                m_tmpPacket.size = 0;
                m_tmpPacket.data = nullptr;
                break;
            }
            //eat up video packets
            if (packet.stream_index != audio_stream_id) {
                qDebug()<<" eat up video packets ";
                if (packet.data)
                    av_free_packet(&packet);
                m_tmpPacket.size = 0;
                continue;
            }
            m_tmpPacket.size = packet.size;
            m_tmpPacket.data = packet.data;
        }
        qDebug()<<"packet size "<<packet.size<<" pts "<<packet.pts<<" dts "<<packet.dts
               <<" streamIndext "<<packet.stream_index<<" side_data_elems "<<packet.side_data_elems
              <<" duration "<<packet.duration<<" pos "<<packet.pos;

        m_output_at = tryDecode ();
        qDebug()<<" get m_output_at "<<m_output_at;

        if (m_output_at < 0) {
            m_output_at = 0;
            m_tmpPacket.size = 0;
            if (ctx->codec_id == AV_CODEC_ID_SHORTEN || ctx->codec_id == AV_CODEC_ID_TWINVQ) {
                if (packet.data)
                    av_free_packet(&packet);
                packet.data = nullptr;
                m_tmpPacket.size = 0;
                break;
            }
            continue;
        } else if (m_output_at == 0) {
            if (packet.data)
                av_free_packet(&packet);
            packet.data = nullptr;
            m_tmpPacket.size = 0;
            continue;
        }
    }
    qDebug()<<" >>>>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" finish <<<<<<<<<<<<<<<<<<< ";
}

qint64 FFmpeg::tryDecode()
{
    int out_size = 0;
    int got_frame = 0;
    qDebug()<<">>>>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<< ";

    qDebug()<<" current m_tmpPacket size is "<<m_tmpPacket.size;
    if (packet.stream_index == audio_stream_id) {
        int l = avcodec_decode_audio4 (ctx, frame, &got_frame, &m_tmpPacket);
        if (l < 0) {
            qCritical()<<Q_FUNC_INFO<<"Decode audio error, error code="<<l;
            return l;
        }
        qDebug()<<" bytes consumed from the input m_tmpPacket is "<<l;

        if (got_frame) {
            out_size = av_samples_get_buffer_size (0, ctx->channels,
                                                   frame->nb_samples, ctx->sample_fmt, 1);
        } else {
            qWarning()<<Q_FUNC_INFO<<"no frame could be decoded, maybe this is an error!!";
            out_size = 0;
        }
        qDebug()<<" required buffer size "<<out_size;
        m_tmpPacket.data += l;
        m_tmpPacket.size -= l;

        qDebug()<<" after m_tmpPacket size is "<<m_tmpPacket.size;
    }
    if (!m_tmpPacket.size && packet.data)
        av_free_packet(&packet);

    qDebug()<<">>>>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" finish <<<<<<<<<<<<<<< ";

    return out_size;
}

AVInputFormat *FFmpeg::dumpSourceFormat(const QString &file)
{
#ifndef SPECIFIC_INPUT_FORMAT
    Q_UNUSED(file)
    return nullptr;
#else
    AVProbeData pd;
    memset (&pd, 0, sizeof(pd));
    unsigned char buf[PROBE_BUFFER_SIZE + AVPROBE_PADDING_SIZE];
    QFile qf(file);
    if (!qf.open (QIODevice::ReadOnly)) {
        qWarning()<<Q_FUNC_INFO<<" >SPECIFIC_INPUT_FORMAT< can't open file !!";
        qf.close ();
        return nullptr;
    }
    pd.filename = file.toUtf8 ().constData ();
    pd.buf_size = qf.peek ((char*)buf, sizeof(buf) - AVPROBE_PADDING_SIZE);
    pd.buf = buf;
    if (pd.buf_size < PROBE_BUFFER_SIZE) {
        qWarning()<<Q_FUNC_INFO<<QString(" >SPECIFIC_INPUT_FORMAT<  too small buffer size: %1 bytes")
                    .arg (pd.buf_size);
        qf.close ();
        return nullptr;
    }
    AVInputFormat *fmt = av_probe_input_format (&pd, 1);
    if(!fmt) {
        qWarning()<<Q_FUNC_INFO<<"usupported format";
        qf.close ();
        return nullptr;
    }
    if (qf.isOpen ())
        qf.close ();
    qDebug()<<Q_FUNC_INFO<<QString("detected format: %1, means [%2]").arg (fmt->long_name).arg (fmt->name);
    return fmt;
#endif
}

//// callbacks
//static int ffmpeg_read(void *data, uint8_t *buf, int size)
//{
//    FFmpeg *d = (FFmpeg*)data;
//    return (int)d->inputSource ()->read ((char*)buf, size);
//}

//static int64_t ffmpeg_seek(void *data, int64_t offset, int whence)
//{
//    FFmpeg *d = (FFmpeg*)data;
//    int64_t absolute_pos = 0;
//    switch( whence )
//    {
//    case AVSEEK_SIZE:
//        return d->inputSource ()->size ();
//    case SEEK_SET:
//        absolute_pos = offset;
//        break;
//    case SEEK_CUR:
//        absolute_pos = d->inputSource ()->pos () + offset;
//        break;
//    case SEEK_END:
//        absolute_pos = d->inputSource ()->size () - offset;
//    default:
//        return -1;
//    }
//    if(absolute_pos < 0 || absolute_pos > d->inputSource ()->size ())
//        return -1;
//    return d->inputSource ()->seek (absolute_pos);
//}

//FFmpeg::FFmpeg(QObject *parent) :
//    IDecoder(parent)
//{
//    avcodec_register_all();
//    avformat_network_init();
//    av_register_all();


//    m_bitrate = 0;
//    m_totalTime = 0;
//    m_formatCtx = nullptr;
//    m_path = fileUri ();
//    m_temp_pkt.size = 0;
//    m_pkt.size = 0;
//    m_pkt.data = 0;
//    m_output_at = 0;
//    m_skipBytes = 0;
//    m_stream = nullptr;
//    m_decoded_frame = nullptr;
//    m_channels = 0;

//    av_init_packet(&m_pkt);
//    av_init_packet(&m_temp_pkt);

//}

//FFmpeg::~FFmpeg()
//{
//    m_bitrate = 0;
//    m_temp_pkt.size = 0;
//    if (m_formatCtx)
//        avformat_free_context(m_formatCtx);
//    if(m_pkt.data)
//        av_free_packet(&m_pkt);
//    if(m_stream)
//        av_free(m_stream);

//    if(m_decoded_frame)
//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100
//        av_frame_free(&m_decoded_frame);
//#else
//        av_free(m_decoded_frame);
//#endif
//}

//bool FFmpeg::initialize()
//{
//    m_bitrate = 0;
//    m_totalTime = 0;
//    m_seekTime = -1;

//    m_formatCtx = avformat_alloc_context();

//    AVProbeData  pd;
//    memset(&pd, 0, sizeof(pd));
//    uint8_t buf[PROBE_BUFFER_SIZE + AVPROBE_PADDING_SIZE];
//    m_path = fileUri ();

//    qDebug()<<Q_FUNC_INFO<<"fileUri "<<m_path;

//    pd.filename = m_path.toUtf8 ().constData();
//    pd.buf_size = inputSource ()->peek ((char*)buf, sizeof(buf) - AVPROBE_PADDING_SIZE);
//    pd.buf = buf;
//    if(pd.buf_size < PROBE_BUFFER_SIZE) {
//        qWarning("DecoderFFmpeg: too small buffer size: %d bytes", pd.buf_size);
//        return false;
//    }
//    AVInputFormat *fmt = av_probe_input_format(&pd, 1);
//    if(!fmt) {
//        qWarning()<<Q_FUNC_INFO<<"DecoderFFmpeg: usupported format";
//        return false;
//    }

//    qDebug()<<Q_FUNC_INFO<<QString("detected format: %1 =%2=").arg (fmt->long_name).arg (fmt->name);

//    m_stream = avio_alloc_context(m_input_buf, INPUT_BUFFER_SIZE, 0, this, ffmpeg_read, NULL, ffmpeg_seek);
//    if(!m_stream) {
//        qWarning()<<Q_FUNC_INFO<<"DecoderFFmpeg: unable to initialize I/O callbacks";
//        return false;
//    }
//    m_stream->seekable = !inputSource ()->isSequential ();
//    m_stream->max_packet_size = INPUT_BUFFER_SIZE;
//    m_formatCtx->pb = m_stream;

//    if(avformat_open_input(&m_formatCtx, 0, fmt, 0) != 0) {
//        qDebug()<<Q_FUNC_INFO<<"DecoderFFmpeg: avformat_open_input() failed";
//        return false;
//    }
//    avformat_find_stream_info(m_formatCtx, 0);
//    if(m_formatCtx->pb)
//        m_formatCtx->pb->eof_reached = 0;

//    m_formatCtx->flags |= AVFMT_FLAG_GENPTS;
//    av_read_play(m_formatCtx);
//    for (wma_idx = 0; wma_idx < (int)m_formatCtx->nb_streams; wma_idx++) {
//        m_codecCtx = m_formatCtx->streams[wma_idx]->codec;
//        if (m_codecCtx->codec_type == AVMEDIA_TYPE_AUDIO)
//            break;
//    }

//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100
//    if (m_codecCtx->channels == 1) {
//        m_codecCtx->request_channel_layout = AV_CH_LAYOUT_MONO;
//        m_channels = m_codecCtx->channels;
//    } else {
//        m_codecCtx->request_channel_layout = AV_CH_LAYOUT_STEREO;
//        m_channels = 2;
//    }
//#else
//    if (m_codecCtx->channels > 0)
//         m_codecCtx->request_channels = qMin(2, m_codecCtx->channels);
//    else
//         m_codecCtx->request_channels = 2;
//    m_channels = m_codecCtx->request_channels;
//#endif

//    av_dump_format(m_formatCtx,0,0,0);

//    AVCodec *codec = avcodec_find_decoder(m_codecCtx->codec_id);

//    if (!codec) {
//        qWarning()<<Q_FUNC_INFO<<"DecoderFFmpeg: unsupported codec for output stream";
//        return false;
//    }

//    if (avcodec_open2(m_codecCtx, codec, 0) < 0) {
//        qWarning()<<Q_FUNC_INFO<<"DecoderFFmpeg: error while opening codec for output stream";
//        return false;
//    }

//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100
//    m_decoded_frame = av_frame_alloc();
//#else
//    m_decoded_frame = avcodec_alloc_frame();
//#endif

//    m_totalTime = inputSource ()->isSequential () ? 0 : m_formatCtx->duration * 1000 / AV_TIME_BASE;

//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
//    if(m_codecCtx->codec_id == AV_CODEC_ID_SHORTEN) //ffmpeg bug workaround
//        m_totalTime = 0;
//#else
//    if(m_codecCtx->codec_id == CODEC_ID_SHORTEN) //ffmpeg bug workaround
//        m_totalTime = 0;
//#endif

//    AudioParameters::AudioFormat format = AudioParameters::PCM_UNKNOWM;

//    switch(m_codecCtx->sample_fmt) {
//    case AV_SAMPLE_FMT_U8:
//    case AV_SAMPLE_FMT_U8P:
//        format = AudioParameters::PCM_S8;
//        break;
//    case AV_SAMPLE_FMT_S16:
//    case AV_SAMPLE_FMT_S16P:
//        format = AudioParameters::PCM_S16LE;
//        break;
//    case AV_SAMPLE_FMT_S32:
//    case AV_SAMPLE_FMT_S32P:
//    case AV_SAMPLE_FMT_FLT:
//    case AV_SAMPLE_FMT_FLTP:
//        format = AudioParameters::PCM_S32LE;
//        break;
//    default:
//        qWarning("DecoderFFmpeg: unsupported audio format");
//        return false;
//    }

//    qDebug()<<Q_FUNC_INFO<<QString("Decoder info sample = [%1], channel = [%2], format = [%3]")
//              .arg (m_codecCtx->sample_rate).arg (m_channels).arg (format);

//    IDecoder::configure(m_codecCtx->sample_rate, m_channels, format);

//    if(m_formatCtx->bit_rate)
//        m_bitrate = m_formatCtx->bit_rate/1000;
//    if(m_codecCtx->bit_rate)
//        m_bitrate = m_codecCtx->bit_rate/1000;

//    qDebug()<<Q_FUNC_INFO<<"initialize succes";
//    qDebug()<<Q_FUNC_INFO<<"total time "<<m_totalTime;
//    return true;
//}

//quint64 FFmpeg::getLength()
//{
//    return m_totalTime;
//}

//void FFmpeg::setPosition(qreal pos)
//{
//    qDebug()<<Q_FUNC_INFO<<"set to pos "<<pos;
//    int64_t timestamp = int64_t(pos)*AV_TIME_BASE/1000;
//    qDebug()<<Q_FUNC_INFO<<"timestamp "<<timestamp;
//    if (m_formatCtx->start_time != (qint64)AV_NOPTS_VALUE)
//        timestamp += m_formatCtx->start_time;
//    m_seekTime = timestamp;
//    av_seek_frame(m_formatCtx, -1, timestamp, AVSEEK_FLAG_BACKWARD);
//    avcodec_flush_buffers(m_codecCtx);
//    av_free_packet(&m_pkt);
//    m_temp_pkt.size = 0;
//}

//int FFmpeg::bitrate()
//{
//    return m_bitrate;
//}

//qint64 FFmpeg::read(char *audio, qint64 maxSize)
//{
//    qDebug()<<Q_FUNC_INFO<<" === read maxSize "<<maxSize<<" m_output_at "<<m_output_at;

//    m_skipBytes = 0;

//    if(!m_output_at)
//        fillBuffer();

//    if(!m_output_at)
//        return 0;
//    qint64 len = qMin(m_output_at, maxSize);

//    qDebug()<<Q_FUNC_INFO<<QString("after fillBuffer m_output_at = [%1], maxSize = [%2], len = [%3]")
//              .arg (m_output_at).arg (maxSize).arg (len);

//    if(av_sample_fmt_is_planar(m_codecCtx->sample_fmt) && m_channels > 1) {
//        int bps = av_get_bytes_per_sample(m_codecCtx->sample_fmt);
//        for(int i = 0; i < (len >> 1); i+=bps) {
//            memcpy(audio + 2*i, m_decoded_frame->extended_data[0] + i, bps);
//            memcpy(audio + 2*i + bps, m_decoded_frame->extended_data[1] + i, bps);
//        }
//        m_output_at -= len;
//        memmove(m_decoded_frame->extended_data[0],
//                m_decoded_frame->extended_data[0] + len/2, m_output_at/2);
//        memmove(m_decoded_frame->extended_data[1],
//                m_decoded_frame->extended_data[1] + len/2, m_output_at/2);
//    } else {
//        memcpy(audio, m_decoded_frame->extended_data[0], len);
//        m_output_at -= len;
//        memmove(m_decoded_frame->extended_data[0], m_decoded_frame->extended_data[0] + len, m_output_at);
//    }

//    if(m_codecCtx->sample_fmt == AV_SAMPLE_FMT_FLTP || m_codecCtx->sample_fmt == AV_SAMPLE_FMT_FLT)
//    {
//        //convert float to signed 32 bit LE
//        for(int i = 0; i < (len >> 2); i++)
//        {
//            int32_t *out = (int32_t *)audio;
//            float *in = (float *) audio;
//            out[i] = qBound(-1.0f, in[i], +1.0f) * (double) 0x7fffffff;
//        }
//    }

//    qDebug()<<Q_FUNC_INFO<<QString("before end m_output_at = [%1], maxSize = [%2], len = [%3]")
//              .arg (m_output_at).arg (maxSize).arg (len);
//    qDebug()<<Q_FUNC_INFO<<" === end ===";

//    return len;
//}

//void FFmpeg::fillBuffer()
//{
//    while(!m_output_at)
//    {
//        qDebug()<<Q_FUNC_INFO<<"loop for !m_output_at, m_temp_pkt.size "<<m_temp_pkt.size;

//        if(!m_temp_pkt.size) {
//            if (av_read_frame(m_formatCtx, &m_pkt) < 0) {
//                qDebug()<<"read the next frame of a stream error or end of file ";
//                m_temp_pkt.size = 0;
//                break;
//            }
//            m_temp_pkt.size = m_pkt.size;
//            m_temp_pkt.data = m_pkt.data;

//            qDebug()<<"m_temp_pkt.size "<<m_temp_pkt.size<<" data "<<m_temp_pkt.data;
//            qDebug()<<"m_pkt.stream_index "<<m_pkt.stream_index<<" wma_idx "<<wma_idx;
//            if(m_pkt.stream_index != wma_idx) {
//                if(m_pkt.data)
//                    av_free_packet(&m_pkt);
//                m_temp_pkt.size = 0;
//                continue;
//            }
//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
//            if(m_seekTime && m_codecCtx->codec_id == AV_CODEC_ID_APE)
//#else
//            if(m_seekTime && m_codecCtx->codec_id == CODEC_ID_APE)
//#endif
//            {
//                int64_t rescaledPts = av_rescale(m_pkt.pts,
//                                                 AV_TIME_BASE * (int64_t)
//                                                 m_formatCtx->streams[m_pkt.stream_index]->time_base.num,
//                                                 m_formatCtx->streams[m_pkt.stream_index]->time_base.den);
//                m_skipBytes =  (m_seekTime - rescaledPts) * m_codecCtx->sample_rate * 4 / AV_TIME_BASE;
//                qDebug()<<" caculate m_skipBytes "<<m_skipBytes;
//            }
//            else
//                m_skipBytes = 0;
//            m_seekTime = 0;
//        }
//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
//        if(m_skipBytes > 0 && m_codecCtx->codec_id == AV_CODEC_ID_APE)
//#else
//        if(m_skipBytes > 0 && m_codecCtx->codec_id == CODEC_ID_APE)
//#endif
//        {
//            while (m_skipBytes > 0) {
//                m_output_at = ffmpeg_decode();
//                if(m_output_at < 0)
//                    break;
//                m_skipBytes -= m_output_at;
//            }
//            qDebug()<<" now m_skipBytes "<<m_skipBytes;
//            if(m_skipBytes < 0) {
//                qint64 size = m_output_at;
//                m_output_at = - m_skipBytes;
//                m_output_at = m_output_at - (m_output_at % 4);

//                if(av_sample_fmt_is_planar(m_codecCtx->sample_fmt) && m_channels > 1) {
//                    memmove(m_decoded_frame->extended_data[0],
//                            m_decoded_frame->extended_data[0] + (size - m_output_at)/2, m_output_at/2);
//                    memmove(m_decoded_frame->extended_data[1],
//                            m_decoded_frame->extended_data[1] + (size - m_output_at)/2, m_output_at/2);
//                } else {
//                    memmove(m_decoded_frame->extended_data[0],
//                            m_decoded_frame->extended_data[0] + size - m_output_at, m_output_at);
//                }
//                m_skipBytes = 0;
//            }
//        }
//        else
//            m_output_at = ffmpeg_decode();

//        qDebug()<<" now m_output_at "<<m_output_at;

//        if(m_output_at < 0) {
//            m_output_at = 0;
//            m_temp_pkt.size = 0;

//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
//            if(m_codecCtx->codec_id == AV_CODEC_ID_SHORTEN || m_codecCtx->codec_id == AV_CODEC_ID_TWINVQ)
//#else
//            if(m_codecCtx->codec_id == CODEC_ID_SHORTEN || m_codecCtx->codec_id == CODEC_ID_TWINVQ)
//#endif
//            {
//                if(m_pkt.data)
//                    av_free_packet(&m_pkt);
//                m_pkt.data = 0;
//                m_temp_pkt.size = 0;
//                break;
//            }
//            continue;
//        }
//        else if(m_output_at == 0)
//        {
//            if(m_pkt.data)
//                av_free_packet(&m_pkt);
//            m_pkt.data = 0;
//            m_temp_pkt.size = 0;
//            continue;
//        }
//    }
//}

//qint64 FFmpeg::ffmpeg_decode()
//{
//    int out_size = 0;
//    int got_frame = 0;
//    if(m_pkt.stream_index == wma_idx) {

//#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100

//#else
//        avcodec_get_frame_defaults(m_decoded_frame);
//#endif

//        int l = avcodec_decode_audio4(m_codecCtx, m_decoded_frame, &got_frame, &m_temp_pkt);

//        if(got_frame)
//            out_size = av_samples_get_buffer_size(0, m_codecCtx->channels, m_decoded_frame->nb_samples,
//                                                  m_codecCtx->sample_fmt, 1);
//        else
//            out_size = 0;

//        if(m_codecCtx->bit_rate)
//            m_bitrate = m_codecCtx->bit_rate/1000;
//        if(l < 0)
//        {
//            return l;
//        }
//        m_temp_pkt.data += l;
//        m_temp_pkt.size -= l;
//    }
//    if (!m_temp_pkt.size && m_pkt.data)
//        av_free_packet(&m_pkt);

//    return out_size;
//}























} //FFmpegDecoder
} //Decoder
} //PhoenixPlayer
