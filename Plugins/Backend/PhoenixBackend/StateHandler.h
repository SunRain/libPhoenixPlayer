#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include <QMutex>
#include <QObject>

#include "PhoenixBackend_global.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

class StateHandler : public QObject
{
    Q_OBJECT
public:
    explicit StateHandler(QObject *parent = Q_NULLPTR);
    virtual ~StateHandler();

     void dispatchElapsed(qint64 currentMilliseconds);
    /*!
     * Sends information about song length
     * @param length song length in milliseconds
     */
    Q_DECL_DEPRECATED void dispatch(qint64 length);

    void dispatchDurationInMS(qint64 ms);

    qint64 durationInMS();

    ///
    /// \brief dispatch Sends playback state.
    /// \param state
    ///
    void dispatch(PlayState state);


    void dispatchSeekTime(qint64 timeMS);

    ///
    /// \brief sendFinished Sends playback finished event.
    ///
    void sendFinished();

    void decodeFinished();

    void requestStopOutput();

    ///
    /// \brief state Returns the current state.
    /// \return
    ///
    PlayState state();

    ///
    /// \brief sendNextTrackRequest Sends next track request.
    ///
    void sendNextTrackRequest();

signals:
    /*!
     * Tracks elapesed time.
     * @param time New track position in milliseconds.
     */
    void elapsedChanged(qint64 time);

    /*!
     * Emitted when bitrate has changed.
     * @param bitrate New bitrate (in kbps)
     */
    void bitrateChanged(int bitrate);
    /*!
     * Emitted when samplerate has changed.
     * @param frequency New sample rate (in Hz)
     */
    void frequencyChanged(quint32 frequency);
    /*!
     * Emitted when sample size has changed.
     * @param size New sample size (in bits)
     */
    void sampleSizeChanged(int size);
    /*!
     * Emitted when channels number has changed.
     * @param channels New channels number.
     */
    void channelsChanged(int channels);
     /*!
     * This signal is emitted when the stream reader fills it's buffer.
     * The argument \b progress indicates the current percentage of buffering completed.
     */
    void bufferingProgress(int progress);

private:
    qint64 m_elapsed;
    qint64 m_length;
    quint32 m_frequency;
    bool m_sendAboutToFinish;
    int m_bitrate, m_precision, m_channels;
    QMutex m_mutex;
    PlayState m_state;
    QList<PlayState> m_clearStates;

};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // STATEHANDLER_H
