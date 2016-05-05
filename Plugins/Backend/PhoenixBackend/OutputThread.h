#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QMutex>

#include "PhoenixBackend_global.h"
#include "AudioParameters.h"

class QAudioOutput;
class QIODevice;

namespace PhoenixPlayer {
//class PluginLoader;
//class PluginHost;
class EqualizerMgr;

namespace OutPut {
class IOutPut;
class OutPutHost;
}

namespace PlayBackend {
//class BaseVolume;
class BaseVisual;

namespace PhoenixBackend {

class RingBuffer;
class OutputThread : public QThread
{
    Q_OBJECT
public:
    explicit OutputThread(RingBuffer *ring, BaseVisual *v = 0, QObject *parent = 0);
    virtual ~OutputThread();

    bool initialize(const PhoenixPlayer::AudioParameters &para);

    bool isPaused();

    void togglePlayPause();

    void reset();
    /*!
     * Requests playback to stop.
     */
    void stop();
    /*!
     * Mutes/Restores volume
     * @param mute state of volume (\b true - mute, \b false - restore)
     */
    void setMuted(bool muted);
    /*!
     * Requests playback to finish.
     */
    void finish();
    /*!
     * Requests a seek to the time \b pos indicated, specified in milliseconds.
     * If \b reset is \b true, this function also clears internal output buffers for faster seeking;
     * otherwise does nothing with buffers.
     */
    void seek(qint64 pos, bool reset = false);

    // QThread interface
protected:
    void run();

private:
    void updateEQ();
private:
//    QAudioOutput *m_output;
    OutPut::OutPutHost *m_outputHost;
    OutPut::IOutPut *m_output;
#if 0
    QIODevice *m_device;
#endif
    EqualizerMgr *m_eq;
//    StateHandler *m_handler;
//    AudioParameters *m_audioParameters;
    BaseVisual *m_visual;
    RingBuffer *m_ring;

    PhoenixPlayer::AudioParameters m_audioParameters;

    QMutex m_mutex;

    unsigned char *m_visBuffer;

    bool m_userStop;
    bool m_pause;
//    bool m_prev_pause;
//    bool m_finish;
    bool m_useEq;
    bool m_muted;
//    bool m_skip;

//    int m_kbps;

//    quint32 m_frequency;
//    qint64 m_bytesPerMillisecond;
//    qint64 m_totalWritten;
//    qint64 m_currentMilliseconds;
    qint64 m_visBufferSize;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // OUTPUTTHREAD_H
