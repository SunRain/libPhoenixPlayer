#include "StateHandler.h"

#include <QMutex>
#include <QDebug>
#include <QCoreApplication>

#include "InternalEvent.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

#define TICK_INTERVAL 500
#define PREFINISH_TIME 7000

StateHandler::StateHandler(QObject *parent)
    : QObject(parent)
    , m_mutex(QMutex::Recursive)
{
    m_elapsed = -1;
    m_length = 0;
    m_bitrate = 0;
    m_frequency = 0;
    m_precision = 0;
    m_channels = 0;
    m_sendAboutToFinish = true;
    m_state = PlayState::Stopped;
}

StateHandler::~StateHandler()
{

}

//currentMilliseconds
void StateHandler::dispatchElapsed(qint64 currentMilliseconds)
{
    m_mutex.lock();
    if (qAbs(m_elapsed - currentMilliseconds) > TICK_INTERVAL) {
        m_elapsed = currentMilliseconds;
        emit elapsedChanged(currentMilliseconds);
    }
    m_mutex.unlock();
}

//StateHandler *StateHandler::instance()
//{
//    static QMutex mutex;
//    static QScopedPointer<StateHandler> scp;

//    if (Q_UNLIKELY(scp.isNull())) {
//        mutex.lock();
//        qDebug()<<QString(">>>>>>> start new %1 <<<<<<<").arg (Q_FUNC_INFO);
//        scp.reset(new StateHandler(0));
//        mutex.unlock();
//    }
//    return scp.data();
//}

//void StateHandler::dispatch(qint64 elapsed, int bitrate, quint32 frequency, int precision, int channels)
//{
//    m_mutex.lock();
//    if (qAbs(m_elapsed - elapsed) > TICK_INTERVAL) {
//        m_elapsed = elapsed;
//        emit (elapsedChanged(elapsed));

//        if (m_bitrate != bitrate) {
//            m_bitrate = bitrate;
//            emit (bitrateChanged(bitrate));
//        }
//        //TODO:  EVENT_NEXT_TRACK_REQUEST
////        if((SoundCore::instance()->totalTime() > PREFINISH_TIME)
////                 && (m_length - m_elapsed < PREFINISH_TIME)
////                 && m_sendAboutToFinish)
////        {
////            m_sendAboutToFinish = false;
////            if(m_length - m_elapsed > PREFINISH_TIME/2)
////                qApp->postEvent(parent(), new QEvent(EVENT_NEXT_TRACK_REQUEST));
////        }
//    }
//    if (m_frequency != frequency) {
//        m_frequency = frequency;
//        emit frequencyChanged(frequency);
//    }
//    if (m_precision != precision) {
//        m_precision = precision;
//        emit sampleSizeChanged(precision);
//    }
//    if (m_channels != channels) {
//        m_channels = channels;
//        emit channelsChanged(channels);
//    }
//    m_mutex.unlock();
//}

void StateHandler::dispatch(qint64 length)
{
    m_mutex.lock();
    m_length = length;
    m_mutex.unlock();
}

void StateHandler::dispatchTrackTimeMS(qint64 ms)
{
    m_mutex.lock();
    m_length = ms;
    m_mutex.unlock();
}

void StateHandler::dispatch(PlayState state)
{
    m_mutex.lock ();
//    QList<PlayState> clearStates;
//    clearStates <<PlayState::Stopped <<PlayState::NormalError <<PlayState::FatalError;
//    if (clearStates.contains (state)) {
//        m_elapsed = -1;
//        m_bitrate = 0;
//        m_frequency = 0;
//        m_precision = 0;
//        m_channels = 0;
//        m_sendAboutToFinish = true;
//    }
//    if (m_state != state) {
//        //TODO maybe should use class in PhoenixBackend_global
////        QStringList states;
////        states << "Playing" << "Paused" << "Stopped" << "Buffering" << "NormalError" << "FatalError";
//        qDebug()<<Q_FUNC_INFO<<QString("Current state: [%1]; previous state: [%2]")
//                  .arg (state).arg (m_state);
//        PlayState prevState = state;
//        m_state = state;
//        qApp->postEvent (parent (), new StateChangedEvent(m_state, prevState));
//    }
    if (m_state != state) {
//        m_state = state;
//        emit stateChanged(m_state);
        qDebug()<<Q_FUNC_INFO<<QString("Current state: [%1]; previous state: [%2]")
                  .arg (state).arg (m_state);
        PlayState prevState = state;
        m_state = state;
        qApp->postEvent (parent (), new StateChangedEvent(m_state, prevState));
    }
    m_mutex.unlock ();
}

void StateHandler::dispatchSeekTime(qint64 timeMS)
{
    qApp->postEvent(parent(), new SeekEvent(timeMS));
}

void StateHandler::sendFinished()
{
    qApp->postEvent (parent (), new QEvent(EVENT_FINISHED));
//    emit finished();
}

PlayState StateHandler::state()
{
    return m_state;
}

void StateHandler::sendNextTrackRequest()
{
    m_mutex.lock();
    if(m_sendAboutToFinish) {
        m_sendAboutToFinish = false;
        qApp->postEvent(parent(), new QEvent(EVENT_NEXT_TRACK_REQUEST));
    }
    m_mutex.unlock();
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
