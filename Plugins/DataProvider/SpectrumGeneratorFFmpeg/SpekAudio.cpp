#include "SpekAudio.h"


extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}

namespace PhoenixPlayer {
namespace DataProvider {
namespace SpectrumGeneratorFFmpeg {

SpekAudio::SpekAudio()
{
    av_register_all();

    av_init_packet(&m_packet);

    m_packet.data = Q_NULLPTR;
    m_packet.size = 0;
    m_offset      = 0;
    m_frame       = av_frame_alloc();
    m_bufferLen   = 0;
    m_bufferPtr   = Q_NULLPTR;

    m_framesPerInterval   = 0;
    m_errorPerInterval    = 0;
    m_errorBase           = 0;

}

SpekAudio::~SpekAudio()
{
    av_lockmgr_register(Q_NULLPTR);

    if (m_bufferPtr) {
        av_freep(&m_bufferPtr);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_packet.data) {
        m_packet.data -= m_offset;
        m_packet.size += m_offset;
        m_offset = 0;
        av_packet_unref(&m_packet);
    }
    if (m_codecContext) {
        avcodec_free_context(&m_codecContext);
    }
    if (m_formatContext) {
        avformat_close_input(&m_formatContext);
    }
}

SpekAudio::AudioError SpekAudio::open(const QString &fileName, int stream)
{
    AudioError error = AudioError::OK;

    AVFormatContext *formatContext = Q_NULLPTR;
    if (avformat_open_input(&formatContext, fileName.toUtf8(), Q_NULLPTR, Q_NULLPTR) != 0) {
        error = AudioError::CANNOT_OPEN_FILE;
    }

    if (!error && avformat_find_stream_info(formatContext, Q_NULLPTR) < 0) {
        // 24-bit APE returns an error but parses the stream info just fine.
        // TODO: old comment, verify
        if (formatContext->nb_streams <= 0) {
            error = AudioError::NO_STREAMS;
        }
    }

    int audioStream = -1;
    int streams     = 0;
    if (!error) {
        for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                if (stream == streams) {
                    audioStream = i;
                }
                streams++;
            }
        }
        if (audioStream == -1) {
            error = AudioError::NO_AUDIO;
        }
    }

    AVStream *avstream          = Q_NULLPTR;
    AVCodec *codec              = Q_NULLPTR;
    AVCodecParameters *codecpar = Q_NULLPTR;
    if (!error) {
        avstream = formatContext->streams[audioStream];
        codecpar = avstream->codecpar;
        codec = avcodec_find_decoder(codecpar->codec_id);
        if (!codec) {
            error = AudioError::NO_DECODER;
        }
    }

//    std::string codec_name;
//    int bit_rate = 0;
//    int sample_rate = 0;
//    int bits_per_sample = 0;
//    int channels = 0;
//    double duration = 0;
    if (!error) {
        // We can already fill in the stream info even if the codec won't be able to open it.
//        codec_name = codec->long_name;
//        bit_rate = codecpar->bit_rate;
        m_sampleRate = codecpar->sample_rate;
//        bits_per_sample = codecpar->bits_per_raw_sample;
//        if (!bits_per_sample) {
//            // APE uses bpcs, FLAC uses bprs.
//            bits_per_sample = codecpar->bits_per_coded_sample;
//        }
//        if (codecpar->codec_id == AV_CODEC_ID_AAC ||
//            codecpar->codec_id == AV_CODEC_ID_MUSEPACK8 ||
//            codecpar->codec_id == AV_CODEC_ID_WMAV1 ||
//            codecpar->codec_id == AV_CODEC_ID_WMAV2) {
//            // These decoders set both bps and bitrate.
//            bits_per_sample = 0;
//        }
//        if (bits_per_sample) {
//            bit_rate = 0;
//        }
        m_channels = codecpar->channels;

        if (avstream->duration != AV_NOPTS_VALUE) {
            m_duration = avstream->duration * av_q2d(avstream->time_base);
        } else if (formatContext->duration != AV_NOPTS_VALUE) {
            m_duration = formatContext->duration / (double) AV_TIME_BASE;
        } else {
            error = AudioError::NO_DURATION;
        }

        if (!error && m_channels <= 0) {
            error = AudioError::NO_CHANNELS;
        }
    }

    AVCodecContext *codecContext = Q_NULLPTR;
    if (!error) {
        error = AudioError::CANNOT_OPEN_DECODER;
        // Allocate a codec context for the decoder.
        codecContext = avcodec_alloc_context3(codec);
        if (codecContext) {
            // Copy codec parameters from input stream to output codec context.
            if (avcodec_parameters_to_context(codecContext, codecpar) == 0) {
                // Finally, init the decoder.
                if (avcodec_open2(codecContext, codec, Q_NULLPTR) == 0) {
                    error = AudioError::OK;
                }
            }
        }
    }

    if (!error) {
        AVSampleFormat fmt = (AVSampleFormat)codecpar->format;
        if (fmt != AV_SAMPLE_FMT_S16 && fmt != AV_SAMPLE_FMT_S16P &&
            fmt != AV_SAMPLE_FMT_S32 && fmt != AV_SAMPLE_FMT_S32P &&
            fmt != AV_SAMPLE_FMT_FLT && fmt != AV_SAMPLE_FMT_FLTP &&
            fmt != AV_SAMPLE_FMT_DBL && fmt != AV_SAMPLE_FMT_DBLP ) {
            error = AudioError::BAD_SAMPLE_FORMAT;
        }
    }

    return error;
}

SpekAudio::AudioError SpekAudio::start(int channel, int samples)
{
    m_requestChannel = channel;
    if (channel < 0 || channel >= m_channels) {
        assert(false);
        error = AudioError::NO_CHANNELS;
        return error;
    }

    AVStream *stream = m_formatContext->streams[m_audioStream];
    int64_t rate = m_sampleRate * (int64_t)stream->time_base.num;
    int64_t duration = (int64_t)(m_duration * stream->time_base.den / stream->time_base.num);
    m_errorBase = samples * (int64_t)stream->time_base.den;
    m_framesPerInterval = av_rescale_rnd(duration, rate, m_errorBase, AV_ROUND_DOWN);
    m_errorPerInterval = (duration * rate) % m_errorBase;

    return AudioError::OK;
}

int SpekAudio::read()
{
    if (!!error) {
        return -1;
    }

    for (;;) {
        while (m_packet.size > 0) {
            av_frame_unref(m_frame);
            int got_frame = 0;
            int len = avcodec_decode_audio4(m_codecContext,
                                            m_frame,
                                            &got_frame,
                                            &m_packet);
            if (len < 0) {
                // Error, skip the frame.
                break;
            }
            m_packet.data += len;
            m_packet.size -= len;
            m_offset += len;
            if (!got_frame) {
                // No data yet, get more frames.
                continue;
            }
            // We have data, return it and come back for more later.
            int samples = m_frame->nb_samples;
            if (samples > m_bufferLen) {
                m_bufferPtr = static_cast<float*>(av_realloc(m_bufferPtr, samples * sizeof(float)));
                m_bufferLen = samples;
            }

            AVSampleFormat format = static_cast<AVSampleFormat>(m_frame->format);
            int is_planar = av_sample_fmt_is_planar(format);
            for (int sample = 0; sample < samples; ++sample) {
                uint8_t *data;
                int offset;
                if (is_planar) {
                    data = m_frame->data[m_requestChannel];
                    offset = sample;
                } else {
                    data = m_frame->data[0];
                    offset = sample * m_channels;
                }
                float value;
                switch (format) {
                case AV_SAMPLE_FMT_S16:
                case AV_SAMPLE_FMT_S16P:
                    value = reinterpret_cast<int16_t*>(data)[offset]
                            / static_cast<float>(INT16_MAX);
                    break;
                case AV_SAMPLE_FMT_S32:
                case AV_SAMPLE_FMT_S32P:
                    value = reinterpret_cast<int32_t*>(data)[offset]
                            / static_cast<float>(INT32_MAX);
                    break;
                case AV_SAMPLE_FMT_FLT:
                case AV_SAMPLE_FMT_FLTP:
                    value = reinterpret_cast<float*>(data)[offset];
                    break;
                case AV_SAMPLE_FMT_DBL:
                case AV_SAMPLE_FMT_DBLP:
                    value = reinterpret_cast<double*>(data)[offset];
                    break;
                default:
                    value = 0.0f;
                    break;
                }
                m_bufferPtr[sample] = value;
            }
            return samples;
        }
        if (m_packet.data) {
            m_packet.data -= m_offset;
            m_packet.size += m_offset;
            m_offset = 0;
            av_packet_unref(&m_packet);
        }

        int res = 0;
        while ((res = av_read_frame(m_formatContext, &m_packet)) >= 0) {
            if (m_packet.stream_index == m_audioStream) {
                break;
            }
            av_packet_unref(&m_packet);
        }
        if (res < 0) {
            // End of file or error.
            return 0;
        }
    }
}

} //SpectrumGeneratorFFmpeg
} //DataProvider
} //PhoenixPlayer
