#ifndef FFMPEG_H
#define FFMPEG_H

#include "Decoder/IDecoder.h"

#include <QObject>

extern "C"{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libavutil/avutil.h>
#include <libavutil/mathematics.h>
}

namespace PhoenixPlayer {
namespace Decoder {
namespace FFmpegDecoder {

#define PROBE_BUFFER_SIZE 8192 // 2*2*512*4
#define INPUT_BUFFER_SIZE 16384

class FFmpeg : public IDecoder
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.Decoder.FFmpegDecoder" FILE "ffmpeg_decoder.json")
    Q_INTERFACES(PhoenixPlayer::Decoder::IDecoder)
public:
    explicit FFmpeg(QObject *parent = 0);

    virtual ~FFmpeg();
    // IDecoder interface
public:
    bool initialize();
    quint64 getLength();
    void setPosition(qreal pos);
//    qreal getPosition();
    int bitrate();
    qint64 read(char *audio, qint64 maxSize);

private:
    //helper functions
    void fillBuffer();
    qint64 ffmpeg_decode();
private:
    AVFormatContext *m_formatCtx;
    AVCodecContext *m_codecCtx;

    int m_bitrate, wma_idx;

    QString m_path;
    qint64 m_totalTime;
    AVPacket m_pkt;
    AVPacket m_temp_pkt;
    qint64 m_output_at;
    uchar m_input_buf[INPUT_BUFFER_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    int64_t m_seekTime;
    qint64  m_skipBytes;
    int m_channels;

    AVIOContext *m_stream;
    AVFrame *m_decoded_frame;

};
} //FFmpegDecoder
} //Decoder
} //PhoenixPlayer
#endif // FFMPEG_H
