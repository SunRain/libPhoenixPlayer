#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H

#include <QMutex>
#include <QThread>

#include "AudioParameters.h"

namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;
class Settings;
class MediaResource;

namespace Decoder {
class IDecoder;
class DecoderHost;
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
class RingBuffer;
class PlayThread : public QThread
{
    Q_OBJECT
public:
    explicit PlayThread(QObject *parent = 0, BaseVisual *v = 0);
    virtual ~PlayThread();

    QMutex *mutex();

    bool play();
    void seek(qint64 millisecond = 0);
    void stop();
    void togglePlayPause();
    void setMuted(bool muted);

    void changeMedia(MediaResource *res = 0, quint64 startSec = 0);

    // QThread interface
    OutputThread *output() const;
//    void setOutput(OutputThread *output);

protected:
    void run();

private:
    void finish();
    void reset();
//    void setAudioParameters(AudioParameters *other);
//    void flush(bool final = false);
    void addOffset();
//    qint64 produceSound(char *data, qint64 size, quint32 brate);
    void createOutput();

private:
    QMutex m_mutex;
    PluginLoader *m_pluginLoader;
    StateHandler *m_handler;
    Settings *m_settings;
    RingBuffer *m_ring;
    BaseVisual *m_visual;
    OutputThread *m_outputThread;
    Decoder::IDecoder *m_decoder;
    Decoder::DecoderHost *m_decoderHost;

//    AudioParameters *m_audioParameters;
//    AudioParameters m_audioParameters;
    PhoenixPlayer::AudioParameters m_audioParameters;

//    bool m_use16BitOutputConvert;
//    AudioParameters *m_convertAudioParameters;

    QStringList m_decoderLibs;
    bool m_next;
    bool m_muted;

    bool m_done;
    bool m_finish;
    bool m_user_stop;
    uint m_bks;
    qint64 m_seekTime;
    qint64 m_startPos;
//    qint64 m_output_at;
//    qint64 m_output_size;
    int m_bitrate;
//    unsigned char *m_output_buf;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // PLAYTHREAD_H
