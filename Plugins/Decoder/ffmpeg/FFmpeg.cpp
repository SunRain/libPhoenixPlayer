#include "FFmpeg.h"

#include <QFile>
#include <QDebug>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
}

#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1: 55.39.100
extern "C"{
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
}
#endif

namespace PhoenixPlayer {
namespace Decoder {
namespace FFmpegDecoder {

// callbacks

static int ffmpeg_read(void *data, uint8_t *buf, int size)
{
    FFmpeg *d = (FFmpeg*)data;
    return (int)d->inputSource ()->read ((char*)buf, size);
}

static int64_t ffmpeg_seek(void *data, int64_t offset, int whence)
{
    FFmpeg *d = (FFmpeg*)data;
    int64_t absolute_pos = 0;
    switch( whence )
    {
    case AVSEEK_SIZE:
        return d->inputSource ()->size ();
    case SEEK_SET:
        absolute_pos = offset;
        break;
    case SEEK_CUR:
        absolute_pos = d->inputSource ()->pos () + offset;
        break;
    case SEEK_END:
        absolute_pos = d->inputSource ()->size () - offset;
    default:
        return -1;
    }
    if(absolute_pos < 0 || absolute_pos > d->inputSource ()->size ())
        return -1;
    return d->inputSource ()->seek (absolute_pos);
}

FFmpeg::FFmpeg(QObject *parent) :
    IDecoder(parent)
{
    avcodec_register_all();
    avformat_network_init();
    av_register_all();


    m_bitrate = 0;
    m_totalTime = 0;
    ic = nullptr;
    m_path = fileUri ();
    m_temp_pkt.size = 0;
    m_pkt.size = 0;
    m_pkt.data = 0;
    m_output_at = 0;
    m_skipBytes = 0;
    m_stream = nullptr;
    m_decoded_frame = nullptr;
    m_channels = 0;

    av_init_packet(&m_pkt);
    av_init_packet(&m_temp_pkt);

}

FFmpeg::~FFmpeg()
{
    m_bitrate = 0;
    m_temp_pkt.size = 0;
    if (ic)
        avformat_free_context(ic);
    if(m_pkt.data)
        av_free_packet(&m_pkt);
    if(m_stream)
        av_free(m_stream);

    if(m_decoded_frame)
#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100
        av_frame_free(&m_decoded_frame);
#else
        av_free(m_decoded_frame);
#endif
}

bool FFmpeg::initialize()
{
    m_bitrate = 0;
    m_totalTime = 0;
    m_seekTime = -1;


    ic = avformat_alloc_context();

    AVProbeData  pd;
    memset(&pd, 0, sizeof(pd));
    uint8_t buf[PROBE_BUFFER_SIZE + AVPROBE_PADDING_SIZE];
    m_path = fileUri ();

    qDebug()<<Q_FUNC_INFO<<"fileUri "<<m_path;

    pd.filename = m_path.toLocal8Bit().constData();
    pd.buf_size = inputSource ()->peek ((char*)buf, sizeof(buf) - AVPROBE_PADDING_SIZE);
    pd.buf = buf;
    if(pd.buf_size < PROBE_BUFFER_SIZE)
    {
        qWarning("DecoderFFmpeg: too small buffer size: %d bytes", pd.buf_size);
        return false;
    }
    AVInputFormat *fmt = av_probe_input_format(&pd, 1);
    if(!fmt)
    {
        qWarning("DecoderFFmpeg: usupported format");
        return false;
    }
    qDebug()<<Q_FUNC_INFO<<QString("detected format: %1 =%2=").arg (fmt->long_name).arg (fmt->name);

    m_stream = avio_alloc_context(m_input_buf, INPUT_BUFFER_SIZE, 0, this, ffmpeg_read, NULL, ffmpeg_seek);
    if(!m_stream)
    {
        qWarning("DecoderFFmpeg: unable to initialize I/O callbacks");
        return false;
    }
    m_stream->seekable = !inputSource ()->isSequential ();
    m_stream->max_packet_size = INPUT_BUFFER_SIZE;
    ic->pb = m_stream;

    if(avformat_open_input(&ic, 0, fmt, 0) != 0)
    {
        qDebug("DecoderFFmpeg: avformat_open_input() failed");
        return false;
    }
    avformat_find_stream_info(ic, 0);
    if(ic->pb)
        ic->pb->eof_reached = 0;

    ic->flags |= AVFMT_FLAG_GENPTS;
    av_read_play(ic);
    for (wma_idx = 0; wma_idx < (int)ic->nb_streams; wma_idx++)
    {
        c = ic->streams[wma_idx]->codec;
        if (c->codec_type == AVMEDIA_TYPE_AUDIO)
            break;
    }

#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100
    if (c->channels == 1)
    {
        c->request_channel_layout = AV_CH_LAYOUT_MONO;
        m_channels = c->channels;
    }
    else
    {
        c->request_channel_layout = AV_CH_LAYOUT_STEREO;
        m_channels = 2;
    }
#else
    if (c->channels > 0)
         c->request_channels = qMin(2, c->channels);
    else
         c->request_channels = 2;
    m_channels = c->request_channels;
#endif

    av_dump_format(ic,0,0,0);

    AVCodec *codec = avcodec_find_decoder(c->codec_id);

    if (!codec)
    {
        qWarning("DecoderFFmpeg: unsupported codec for output stream");
        return false;
    }

    if (avcodec_open2(c, codec, 0) < 0)
    {
        qWarning("DecoderFFmpeg: error while opening codec for output stream");
        return false;
    }

#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100
    m_decoded_frame = av_frame_alloc();
#else
    m_decoded_frame = avcodec_alloc_frame();
#endif

    m_totalTime = inputSource ()->isSequential () ? 0 : ic->duration * 1000 / AV_TIME_BASE;

#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
    if(c->codec_id == AV_CODEC_ID_SHORTEN) //ffmpeg bug workaround
        m_totalTime = 0;
#else
    if(c->codec_id == CODEC_ID_SHORTEN) //ffmpeg bug workaround
        m_totalTime = 0;
#endif

    AudioParameters::AudioFormat format = AudioParameters::PCM_UNKNOWM;

    switch(c->sample_fmt)
    {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
        format = AudioParameters::PCM_S8;
        break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
        format = AudioParameters::PCM_S16LE;
        break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
        format = AudioParameters::PCM_S32LE;
        break;
    default:
        qWarning("DecoderFFmpeg: unsupported audio format");
        return false;
    }

    qDebug()<<Q_FUNC_INFO<<QString("Decoder info sample = [%1], channel = [%2], format = [%3]")
              .arg (c->sample_rate).arg (m_channels).arg (format);

    IDecoder::configure(c->sample_rate, m_channels, format);

    if(ic->bit_rate)
        m_bitrate = ic->bit_rate/1000;
    if(c->bit_rate)
        m_bitrate = c->bit_rate/1000;

    qDebug()<<Q_FUNC_INFO<<"initialize succes";
    qDebug()<<Q_FUNC_INFO<<"total time "<<m_totalTime;
    return true;
}

quint64 FFmpeg::getLength()
{
    return m_totalTime;
}

void FFmpeg::setPosition(qreal pos)
{
    qDebug()<<Q_FUNC_INFO<<"set to pos "<<pos;
    int64_t timestamp = int64_t(pos)*AV_TIME_BASE/1000;
    qDebug()<<Q_FUNC_INFO<<"timestamp "<<timestamp;
    if (ic->start_time != (qint64)AV_NOPTS_VALUE)
        timestamp += ic->start_time;
    m_seekTime = timestamp;
    av_seek_frame(ic, -1, timestamp, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(c);
    av_free_packet(&m_pkt);
    m_temp_pkt.size = 0;
}

int FFmpeg::bitrate()
{
    return m_bitrate;
}

qint64 FFmpeg::read(char *audio, qint64 maxSize)
{
//    qDebug()<<Q_FUNC_INFO<<" === read maxSize "<<maxSize<<" m_output_at "<<m_output_at;

    m_skipBytes = 0;

    if(!m_output_at)
        fillBuffer();

    if(!m_output_at)
        return 0;
    qint64 len = qMin(m_output_at, maxSize);

//    qDebug()<<Q_FUNC_INFO<<QString("after fillBuffer m_output_at = [%1], maxSize = [%2], len = [%3]")
//              .arg (m_output_at).arg (maxSize).arg (len);

    if(av_sample_fmt_is_planar(c->sample_fmt) && m_channels > 1)
    {
        int bps = av_get_bytes_per_sample(c->sample_fmt);
        for(int i = 0; i < (len >> 1); i+=bps)
        {
            memcpy(audio + 2*i, m_decoded_frame->extended_data[0] + i, bps);
            memcpy(audio + 2*i + bps, m_decoded_frame->extended_data[1] + i, bps);
        }
        m_output_at -= len;
        memmove(m_decoded_frame->extended_data[0],
                m_decoded_frame->extended_data[0] + len/2, m_output_at/2);
        memmove(m_decoded_frame->extended_data[1],
                m_decoded_frame->extended_data[1] + len/2, m_output_at/2);
    }
    else
    {
        memcpy(audio, m_decoded_frame->extended_data[0], len);
        m_output_at -= len;
        memmove(m_decoded_frame->extended_data[0], m_decoded_frame->extended_data[0] + len, m_output_at);
    }

    if(c->sample_fmt == AV_SAMPLE_FMT_FLTP || c->sample_fmt == AV_SAMPLE_FMT_FLT)
    {
        //convert float to signed 32 bit LE
        for(int i = 0; i < (len >> 2); i++)
        {
            int32_t *out = (int32_t *)audio;
            float *in = (float *) audio;
            out[i] = qBound(-1.0f, in[i], +1.0f) * (double) 0x7fffffff;
        }
    }

//    qDebug()<<Q_FUNC_INFO<<QString("before end m_output_at = [%1], maxSize = [%2], len = [%3]")
//              .arg (m_output_at).arg (maxSize).arg (len);
//    qDebug()<<Q_FUNC_INFO<<" === end ===";

    return len;
}

void FFmpeg::fillBuffer()
{
    while(!m_output_at)
    {
//        qDebug()<<Q_FUNC_INFO<<"loop for !m_output_at, m_temp_pkt.size "<<m_temp_pkt.size;

        if(!m_temp_pkt.size)
        {
            if (av_read_frame(ic, &m_pkt) < 0) {
                qDebug()<<"read the next frame of a stream error or end of file ";
                m_temp_pkt.size = 0;
                break;
            }
            m_temp_pkt.size = m_pkt.size;
            m_temp_pkt.data = m_pkt.data;

            if(m_pkt.stream_index != wma_idx)
            {
                if(m_pkt.data)
                    av_free_packet(&m_pkt);
                m_temp_pkt.size = 0;
                continue;
            }
#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
            if(m_seekTime && c->codec_id == AV_CODEC_ID_APE)
#else
            if(m_seekTime && c->codec_id == CODEC_ID_APE)
#endif
            {
                int64_t rescaledPts = av_rescale(m_pkt.pts,
                                                 AV_TIME_BASE * (int64_t)
                                                 ic->streams[m_pkt.stream_index]->time_base.num,
                                                 ic->streams[m_pkt.stream_index]->time_base.den);
                m_skipBytes =  (m_seekTime - rescaledPts) * c->sample_rate * 4 / AV_TIME_BASE;
            }
            else
                m_skipBytes = 0;
            m_seekTime = 0;
        }
#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
        if(m_skipBytes > 0 && c->codec_id == AV_CODEC_ID_APE)
#else
        if(m_skipBytes > 0 && c->codec_id == CODEC_ID_APE)
#endif
        {
            while (m_skipBytes > 0)
            {
                m_output_at = ffmpeg_decode();
                if(m_output_at < 0)
                    break;
                m_skipBytes -= m_output_at;
            }

            if(m_skipBytes < 0)
            {
                qint64 size = m_output_at;
                m_output_at = - m_skipBytes;
                m_output_at = m_output_at - (m_output_at % 4);

                if(av_sample_fmt_is_planar(c->sample_fmt) && m_channels > 1)
                {
                    memmove(m_decoded_frame->extended_data[0],
                            m_decoded_frame->extended_data[0] + (size - m_output_at)/2, m_output_at/2);
                    memmove(m_decoded_frame->extended_data[1],
                            m_decoded_frame->extended_data[1] + (size - m_output_at)/2, m_output_at/2);
                }
                else
                {
                    memmove(m_decoded_frame->extended_data[0],
                            m_decoded_frame->extended_data[0] + size - m_output_at, m_output_at);
                }
                m_skipBytes = 0;
            }
        }
        else
            m_output_at = ffmpeg_decode();

//        qDebug()<<" now m_output_at "<<m_output_at;

        if(m_output_at < 0)
        {
            m_output_at = 0;
            m_temp_pkt.size = 0;

#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav 10
            if(c->codec_id == AV_CODEC_ID_SHORTEN || c->codec_id == AV_CODEC_ID_TWINVQ)
#else
            if(c->codec_id == CODEC_ID_SHORTEN || c->codec_id == CODEC_ID_TWINVQ)
#endif
            {
                if(m_pkt.data)
                    av_free_packet(&m_pkt);
                m_pkt.data = 0;
                m_temp_pkt.size = 0;
                break;
            }
            continue;
        }
        else if(m_output_at == 0)
        {
            if(m_pkt.data)
                av_free_packet(&m_pkt);
            m_pkt.data = 0;
            m_temp_pkt.size = 0;
            continue;
        }
    }
}

qint64 FFmpeg::ffmpeg_decode()
{
    int out_size = 0;
    int got_frame = 0;
    if(m_pkt.stream_index == wma_idx)
    {

#if (LIBAVCODEC_VERSION_INT >= ((55<<16)+(34<<8)+0)) //libav-10: 55.34.1; ffmpeg-2.1:  55.39.100

#else
        avcodec_get_frame_defaults(m_decoded_frame);
#endif

        int l = avcodec_decode_audio4(c, m_decoded_frame, &got_frame, &m_temp_pkt);

        if(got_frame)
            out_size = av_samples_get_buffer_size(0, c->channels, m_decoded_frame->nb_samples,
                                                  c->sample_fmt, 1);
        else
            out_size = 0;

        if(c->bit_rate)
            m_bitrate = c->bit_rate/1000;
        if(l < 0)
        {
            return l;
        }
        m_temp_pkt.data += l;
        m_temp_pkt.size -= l;
    }
    if (!m_temp_pkt.size && m_pkt.data)
        av_free_packet(&m_pkt);

    return out_size;
}























} //FFmpegDecoder
} //Decoder
} //PhoenixPlayer
