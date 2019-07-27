#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "AudioParameters.h"

namespace PhoenixPlayer {
    class EqualizerMgr;
    class PluginLoader;
    class PluginHost;
    class PPSettings;

    namespace OutPut {
        class IOutPut;
        class OutPutHost;
    }

    namespace PlayBackend {
        class BaseVisual;

        namespace PhoenixBackend {
            class StateHandler;
            class BufferQueue;
            class AudioConverter;
            class ChannelConverter;
            class AudioEffect;

class OutputThread : public QThread
{
    Q_OBJECT
public:
    explicit OutputThread(StateHandler *handler,
                          BufferQueue *queue,
//                          QList<AudioEffect*> *list,
                          QObject *parent = Q_NULLPTR);

    virtual ~OutputThread() Q_DECL_OVERRIDE;

    bool initialization(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &list);

    bool isPaused();

    void togglePlayPause();

    void reset();

    void stop();

    void setMuted(bool muted);

    void finish();

    // QObject interface
public:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;

private:
    void updateEQ();
    void status();

private:
    StateHandler                *m_handler              = Q_NULLPTR;
    BufferQueue                 *m_bufferQueue          = Q_NULLPTR;
    AudioConverter              *m_format_converter     = Q_NULLPTR;
    ChannelConverter            *m_channel_converter    = Q_NULLPTR;
//    QList<AudioEffect *>        *m_effectList         = Q_NULLPTR;

    EqualizerMgr                *m_eq = Q_NULLPTR;
    bool m_useEq = false;


    OutPut::OutPutHost          *m_outputHost   = Q_NULLPTR;
    OutPut::IOutPut             *m_output       = Q_NULLPTR;

    unsigned char               *m_output_buf = Q_NULLPTR;
    size_t                      m_output_size = 0;

    qint64 m_bytesPerMillisecond    = -1;
    qint64 m_totalWritten           = -1;
    qint64 m_currentMilliseconds    = -1;

    AudioParameters         m_in_params;

    quint32                 m_sampleRate = 0;
    QList<AudioParameters::ChannelPosition> m_channels;
    AudioParameters::AudioFormat            m_format = AudioParameters::PCM_UNKNOWN;

    bool m_paused = false;
    bool m_user_stop = false;
    bool m_muted = false;
    bool m_finish = false;

    QMutex m_mutex;
    QWaitCondition m_pauseWait;


};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // OUTPUTTHREAD_H
