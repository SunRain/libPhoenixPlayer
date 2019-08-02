#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QList>

#include "AudioParameters.h"

namespace PhoenixPlayer {
    class PluginLoader;
    class PluginHost;
    class PPSettings;
    class MediaResource;

    namespace Decoder {
        class IDecoder;
        class DecoderHost;
    }

    namespace PlayBackend {
        class BaseMediaObject;
        class BaseVisual;

        namespace PhoenixBackend {
            class StateHandler;
            class BufferQueue;
            class AudioConverter;
            class AudioEffect;
            class ChannelConverter;

class DecodeThread : public QThread
{
    Q_OBJECT
public:
    DecodeThread(StateHandler *handle,
                 BufferQueue *queue,
//                 AudioConverter *converter,
                 QList<AudioEffect*> *list,
                 QObject *parent = Q_NULLPTR);

    virtual ~DecodeThread() Q_DECL_OVERRIDE;

    bool changeMedia(MediaResource *res = Q_NULLPTR, quint64 startSec = 0);

    void stopDecoding();

    void seekMS(qint64 millisecond = 0);

    AudioParameters audioParameters() const;

    // QObject interface
public:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;

private:
    void reset();

private:
    StateHandler                *m_handler          = Q_NULLPTR;
    BufferQueue                 *m_bufferQueue      = Q_NULLPTR;
    AudioConverter              *m_audioConverter   = Q_NULLPTR;
    ChannelConverter            *m_channelConverter = Q_NULLPTR;
    QList<AudioEffect *>        *m_effectList       = Q_NULLPTR;

    PPSettings                  *m_settings     = Q_NULLPTR;
    PluginLoader                *m_pluginLoader = Q_NULLPTR;
    MediaResource               *m_resource     = Q_NULLPTR;

    Decoder::IDecoder           *m_decoder      = Q_NULLPTR;
    Decoder::DecoderHost        *m_decoderHost  = Q_NULLPTR;

    QStringList m_decoderLibs;

    PhoenixPlayer::AudioParameters m_audioParameters;

    QMutex  m_mutex;

    qint64 m_seekTimeMS = -1; //seek time in milliseconds;
    bool m_user_stop = false;
//    bool m_finish = false;
//    bool m_done = false;

    uint m_blockSize = 0;
    uint m_sample_size = 0;
    int m_bitrate = 0;
    unsigned char *m_output_buf = Q_NULLPTR;
    quint64 m_outputSize = 0;
};



} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // DECODETHREAD_H
