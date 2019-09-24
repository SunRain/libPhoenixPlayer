#ifndef FFMPEG_H
#define FFMPEG_H

#include "IDecoder.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
}

#include <QObject>

#include "AudioParameters.h"

namespace PhoenixPlayer {

    namespace PlayBackend {

        namespace PhoenixBackend {

            namespace Decoder {

                namespace FFmpegDecoder {

//#define PROBE_BUFFER_SIZE 8192 // 2*2*512*4
//#define INPUT_BUFFER_SIZE 16384

class FFmpeg : public IDecoder
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "PhoenixPlayer.Decoder.FFmpegDecoder" FILE "ffmpeg_decoder.json")
//    Q_INTERFACES(PhoenixPlayer::Decoder::IDecoder)
public:
    explicit FFmpeg(QObject *parent = Q_NULLPTR);

    virtual ~FFmpeg() override;

    // IDecoder interface
public:
    bool initialize(MediaResource *res) Q_DECL_OVERRIDE;
    qint64 durationInSeconds() Q_DECL_OVERRIDE;
    void setPositionMS(qreal millisecond) Q_DECL_OVERRIDE;
    int bitrate() Q_DECL_OVERRIDE;
    qint64 read(unsigned char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    AudioParameters audioParameters() const Q_DECL_OVERRIDE;

private:
    void reset();
    void close();
    void fillBuffer();
    qint64 tryDecode();
    AVInputFormat *dumpSourceFormat(const QString &file);

private:
    AudioParameters::AudioFormat m_audioFormat;
    AudioParameters m_parameter;

    AVFormatContext *ic;
    AVCodecContext *c;
//    AVSampleFormat m_decoded_frame;

    AVFrame *m_decoded_frame;
    AVPacket m_pkt;
    AVPacket m_temp_pkt;
//    AVStream *audio_st;

    MediaResource *m_resource;

    qint64 current_in_seconds;
    qint64 duration_in_seconds;
    qint64 m_output_at;
    qint64 m_skipBytes = 0;

    int64_t m_seekTime;

    int audioIndex;
    int m_bitrate;
    int m_channels = 0;
};
} //FFmpegDecoder
} //Decoder
} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // FFMPEG_H
