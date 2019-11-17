#ifndef SPEKAUDIO_H
#define SPEKAUDIO_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}

#include <QString>

namespace PhoenixPlayer {
namespace DataProvider {
namespace SpectrumGeneratorFFmpeg {


class SpekAudio
{
public:
    enum class AudioError
    {
        OK,
        CANNOT_OPEN_FILE,
        NO_STREAMS,
        NO_AUDIO,
        NO_DECODER,
        NO_DURATION,
        NO_CHANNELS,
        CANNOT_OPEN_DECODER,
        BAD_SAMPLE_FORMAT,
    };

    explicit SpekAudio();
    virtual ~SpekAudio();

    AudioError open(const QString &fileName, int stream);

    AudioError start(int channel, int samples);

    int read();

    inline AudioError getError() const
    {
        return error;
    }

    inline const float *getBuffer() const
    {
        return m_bufferPtr;
    }

    inline int64_t getErrorBase() const
    {
        return m_errorBase;
    }

    inline int64_t getFramesPerInterval() const
    {
        return m_framesPerInterval;
    }

    inline int64_t getErrorPerInterval() const
    {
        return m_errorPerInterval;
    }
private:
    AudioError          error;

    AVFormatContext     *m_formatContext    = Q_NULLPTR;
    AVCodecContext      *m_codecContext     = Q_NULLPTR;
    AVFrame             *m_frame            = Q_NULLPTR;

    float               *m_bufferPtr        = Q_NULLPTR;

    int                 m_audioStream;
    int                 m_requestChannel;
    int                 m_offset;
    int                 m_bufferLen;
//    int                 m_bitRate;
    int                 m_sampleRate;
//    int                 m_bitsPerSample;
    int                 m_streams;
    int                 m_channels;
    double              m_duration;

//    std::string         m_codecName;

    AVPacket            m_packet;

    // TODO: these guys don't belong here, move them somewhere else when revamping the pipeline
    int64_t             m_framesPerInterval;
    int64_t             m_errorPerInterval;
    int64_t             m_errorBase;
};


} //SpectrumGeneratorFFmpeg
} //DataProvider
} //PhoenixPlayer


inline bool operator!(PhoenixPlayer::DataProvider::SpectrumGeneratorFFmpeg::SpekAudio::AudioError error) {
    return error == PhoenixPlayer::DataProvider::SpectrumGeneratorFFmpeg::SpekAudio::AudioError::OK;
}

inline std::ostream& operator<<(std::ostream& os,
                                PhoenixPlayer::DataProvider::SpectrumGeneratorFFmpeg::SpekAudio::AudioError error) {
    return os << static_cast<int>(error);
}

#endif // SPEKAUDIO_H
