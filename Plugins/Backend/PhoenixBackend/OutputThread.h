#ifndef OUTPUTTHREAD_H
#define OUTPUTTHREAD_H

#include <QThread>
#include <QMutex>

#include "AudioParameters.h"

#include "PhoenixBackend_global.h"
#include "Recycler.h"

namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;
class EqualizerMgr;

namespace OutPut {
class IOutPut;
class OutPutHost;
}
namespace PlayBackend {
//class BaseVolume;
class BaseVisual;
namespace PhoenixBackend {
class StateHandler;
//class Recycler;
class OutputThread : public QThread
{
    Q_OBJECT
public:
    explicit OutputThread(QObject *parent = 0, BaseVisual *v = 0);
    virtual ~OutputThread();

    bool initialize(quint32 freq, int chan, AudioParameters::AudioFormat format);

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
    /*!
     * Returns Recycler pointer.
     */
    Recycler *recycler();
    /*!
     * Returns mutex pointer.
     */
    QMutex *mutex();
    /*!
     * Returns selected audio parameters.
     */
    AudioParameters *audioParameters();

    // QThread interface
protected:
    void run();

signals:

private:
    void dispatch(const PlayState &state);
    void dispatch(qint64 elapsed,
                  int bitrate,
                  int frequency,
                  int bits,
                  int channels);
    void status();
    void clearVisual();
    void dispatchVisual(Buffer *buffer);
    void updateEQ();
private:
    bool m_userStop;
    bool m_pause;
    bool m_prev_pause;
    bool m_finish;
    bool m_useEq;
    bool m_muted;
    bool m_skip;

    quint32 m_frequency;
    int m_kbps;

    qint64 m_bytesPerMillisecond;

    qint64 m_totalWritten, m_currentMilliseconds;

    QMutex m_mutex;
    Recycler m_recycler;

    OutPut::OutPutHost *m_outputHost;
    OutPut::IOutPut *m_output;

    EqualizerMgr *m_eq;
    StateHandler *m_handler;
    AudioParameters *m_audioParameters;
    BaseVisual *m_visual;

    unsigned char *m_visBuffer;
    qint64 m_visBufferSize;

    PluginLoader *m_pluginLoader;
//    PluginHost *m_pluginHost;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // OUTPUTTHREAD_H
