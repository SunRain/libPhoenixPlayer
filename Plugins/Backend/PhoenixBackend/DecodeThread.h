#ifndef DECODETHREAD_H
#define DECODETHREAD_H

#include <QMutex>
#include <QThread>

#include "AudioParameters.h"

class QTimer;
namespace PhoenixPlayer {
    class PluginLoader;
    class PluginHost;
    class PPSettings;
    class MediaResource;

    namespace PlayBackend {
        class BaseMediaObject;
        class BaseVisual;

        namespace PhoenixBackend {
            class Recycler;
            class StateHandler;
            class OutputThread;
            class ReplayGain;
            class AudioConverter;
            class Dithering;
            class OutputThread;
            class ChannelConverter;
            class AudioEffect;
            class Recycler;

            namespace Decoder {
                class IDecoder;
            }

class DecodeThread : public QThread
{
    Q_OBJECT
public:
    explicit DecodeThread(StateHandler *handle, Recycler *recycler, QObject *parent = Q_NULLPTR);
    virtual ~DecodeThread() override;

    bool initialization(MediaResource *res, qint64 startTimeMS = 0);

    void play();
    void seek(qint64 millisecond = 0);
    void stop();
//    void togglePlayPause();
//    void setMuted(bool muted);

//    bool changeMedia(MediaResource *res = Q_NULLPTR, qint64 startSec = 0);

    AudioParameters audioParameters() const;

    // QObject interface
public:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;


private:
    void flush(bool final = false);
    void reset();
    void prepareEffects(Decoder::IDecoder *d);
    qint64 produceSound(unsigned char *data, qint64 size, quint32 brate);

//    OutputWriter *createOutput();

    inline QMutex *mutex()
    {
        return &m_mutex;
    }

private:
    StateHandler                            *m_handler          = Q_NULLPTR;
    Recycler                                *m_recycler         = Q_NULLPTR;
    ReplayGain                              *m_replayGain       = Q_NULLPTR;
    AudioConverter                          *m_converter        = Q_NULLPTR;
    Dithering                               *m_dithering        = Q_NULLPTR;
    ChannelConverter                        *m_channelConverter = Q_NULLPTR;

    unsigned char                           *m_output_buf       = Q_NULLPTR;

    Decoder::IDecoder                       *m_decoder          = Q_NULLPTR;

    MediaResource                           *m_resource         = Q_NULLPTR;

    QList<AudioEffect*> m_effects;

    QMutex              m_mutex;
    AudioParameters     m_ap;

    std::atomic_bool    m_done;
    std::atomic_bool    m_finish;
    std::atomic_bool    m_user_stop;
    int                 m_bitrate;
    uint                m_bks;
    uint                m_sample_size;
    qint64              m_seekTimeMS;
    quint64             m_output_at;
    quint64             m_output_size;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // DECODETHREAD_H
