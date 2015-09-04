#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QMutex>
#include <QThread>

#include "AudioParameters.h"

namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;

namespace Decoder {
class IDecoder;
}
namespace OutPut {
class IOutPut;
}
namespace PlayBackend {
class BaseMediaObject;
class BaseVisual;
namespace PhoenixBackend {
class Recycler;
class StateHandler;
class OutputThread;
class PlayThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = 0, BaseVisual *v = 0);
    virtual ~PlayThread();

    QMutex *mutex();

    bool play();
    void seek(qint64 time);
    void stop();
    void pause();
    void setMuted(bool muted);

    void changeMedia(PlayBackend::BaseMediaObject *obj = 0, quint64 startSec = 0);

    // QThread interface
protected:
    void run();

private:
    void finish();
    void reset();
    void setAudioParameters(AudioParameters *other);
    void flush(bool final = false);
    void addOffset();
    qint64 produceSound(char *data, qint64 size, quint32 brate);
    OutputThread *createOutput();

private:
    QMutex m_mutex;
    PluginLoader *m_pluginLoader;
    StateHandler *m_handler;

    BaseVisual *m_visual;
    OutputThread *m_output;
    Decoder::IDecoder *m_decoder;

    AudioParameters *m_audioParameters;

    bool m_use16BitOutputConvert;
    AudioParameters *m_convertAudioParameters;

    bool m_next;
    bool m_muted;

    bool m_done;
    bool m_finish;
    bool m_user_stop;
    uint m_bks;
    qint64 m_seekTime;
    qint64 m_startPos;
    qint64 m_output_at;
    qint64 m_output_size;
    int m_bitrate;
    unsigned char *m_output_buf;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // PLAYTHREAD_H
