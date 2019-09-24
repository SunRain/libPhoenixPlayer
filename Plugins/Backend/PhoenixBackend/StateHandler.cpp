#include "StateHandler.h"

#include <QMutex>
#include <QDebug>
#include <QCoreApplication>

#include "InternalEvent.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

const static int TICK_INTERVAL = 500;

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

    m_clearStates << PlayState::Stopped <<PlayState::NormalError << PlayState::FatalError;
}

StateHandler::~StateHandler()
{

}

void StateHandler::dispatchElapsed(qint64 currentMilliseconds)
{
    m_mutex.lock();
    if (qAbs(m_elapsed - currentMilliseconds) > TICK_INTERVAL) {
        m_elapsed = currentMilliseconds;
        emit elapsedChanged(currentMilliseconds);
    }
    m_mutex.unlock();
}

void StateHandler::dispatch(qint64 length)
{
    m_mutex.lock();
    m_length = length;
    m_mutex.unlock();
}

void StateHandler::dispatchDurationInMS(qint64 ms)
{
    m_mutex.lock();
    m_length = ms;
    m_mutex.unlock();
}

qint64 StateHandler::durationInMS()
{
    return m_length;
}

void StateHandler::dispatch(PlayState state)
{
    m_mutex.lock ();
    if (m_clearStates.contains(state)) {
        m_elapsed = -1;
        m_bitrate = 0;
        m_frequency = 0;
        m_precision = 0;
        m_channels = 0;
        m_sendAboutToFinish = true;
    }
    if (m_state != state) {
        qDebug()<<Q_FUNC_INFO<<QString("Current state: [%1]; previous state: [%2]")
                                       .arg(playStateToName(state)).arg(playStateToName(m_state));
        PlayState prevState = state;
        m_state = state;
        qApp->postEvent(parent(), new StateChangedEvent(m_state, prevState));
    }
    m_mutex.unlock ();
}

void StateHandler::dispatchSeekTime(qint64 timeMS)
{
    qApp->postEvent(parent(), new SeekEvent(timeMS));
}

void StateHandler::sendFinished()
{
    qApp->postEvent(parent (), new QEvent(EVENT_FINISHED));
    //    emit finished();
}

void StateHandler::decodeFinished()
{
    qApp->postEvent(parent(), new QEvent(EVENT_DECODE_FINISHED));
}

void StateHandler::requestStopOutput()
{
    qApp->postEvent(parent(), new QEvent(EVENT_REQUEST_STOP_OUTPUT));
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
