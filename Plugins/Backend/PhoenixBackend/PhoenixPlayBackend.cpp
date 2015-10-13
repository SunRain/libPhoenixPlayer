#include "PhoenixPlayBackend.h"

#include <QDebug>
#include <QCoreApplication>

#include "StateHandler.h"
#include "StateChangedEvent.h"
#include "PlayThread.h"
#include "OutputThread.h"
#include "StateChangedEvent.h"

#include "Common.h"
#include "PlayerCore/VolumeControl.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

PhoenixPlayBackend::PhoenixPlayBackend(QObject *parent) :
    IPlayBackend(parent)
{
    m_engine = nullptr;
    m_handler = nullptr;
    m_volumeControl = nullptr;
    m_muted = false;
}

PhoenixPlayBackend::~PhoenixPlayBackend()
{
    stop ();
}

//    ///
//    /// \brief finished 当前曲目结束
//    ///
//    void finished();
//    ///
//    /// \brief failed 当前曲目播放失败
//    ///
//    void failed();

//    void stateChanged(Common::PlayBackendState state);
//    void tick(quint64 sec = 0);
bool PhoenixPlayBackend::event(QEvent *e)
{
    if (e->type () == EVENT_STATE_CHANGED) {
        PlayState st = ((StateChangedEvent *)e)->currentState ();

//        QStringList states;
//        states << "Playing" << "Paused" << "Stopped" << "Buffering" << "NormalError" << "FatalError";
        qDebug()<<Q_FUNC_INFO<<"Current event is EVENT_STATE_CHANGED, value is "<<st;

        if (st == PlayState::Playing)
            emit stateChanged (Common::PlayBackendPlaying);
        else if (st == PlayState::Paused)
            emit stateChanged (Common::PlayBackendPaused);
        else if (st == PlayState::NormalError || st == PlayState::FatalError)
            emit failed ();
        else
            emit stateChanged (Common::PlayBackendStopped);
    } else if (e->type () == EVENT_NEXT_TRACK_REQUEST
               || e->type () == EVENT_FINISHED) {
        emit finished ();
    } else {
        return QObject::event (e);
    }
    return true;
}

Common::PlayBackendState PhoenixPlayBackend::playBackendState()
{
//    PlayBackendStopped = 0x0,  //播放停止
//    PlayBackendPlaying,        //播放
//    PlayBackendPaused          //暂停

//    Playing = 0x1000,   //The player is playing source
//    Paused,             //The player has currently paused its playback
//    Stopped,            //The player is ready to play source
//    Buffering,          //The Player is waiting for data to be able to start playing.
//    NormalError,        //Input source is invalid or unsupported. Player should skip this file
//    FatalError          //This means unrecorvable error die audio output problems. Player should abort playback.
    if (m_handler->state () == PlayState::Playing) {
        return Common::PlayBackendPlaying;
    } else if (m_handler->state () == PlayState::Paused) {
        return Common::PlayBackendPaused;
    } else {
        return Common::PlayBackendStopped;
    }
}

void PhoenixPlayBackend::init()
{
    m_engine = new PlayThread(this, getVisual ());
    m_handler = StateHandler::instance ();
    //set StateHandler's parent to receive dispatch event
    m_handler->setParent (this);
    m_volumeControl = VolumeControl::instance ();

    connect (m_volumeControl, &VolumeControl::mutedChanged,
             [&] (bool muted) {
        m_engine->setMuted (muted);
    });

    connect (m_handler, &StateHandler::elapsedChanged,
             [&](qint64 time) {
        emit tick(time/1000);
    });

    connect (m_handler, &StateHandler::bitrateChanged,
             [&](int bitrate) {
        //TODO bitrateChanged
    });

    connect (m_handler, &StateHandler::frequencyChanged,
             [&](quint32 frequency) {
        //TODO frequencyChanged
    });

    connect (m_handler, &StateHandler::sampleSizeChanged,
             [&](int size) {
        //TODO sampleSizeChanged
    });

    connect (m_handler, &StateHandler::channelsChanged,
             [&](int channels) {
        //TODO channelsChanged
    });

    connect (m_handler, &StateHandler::bufferingProgress,
             [&](int progress) {
        //TODO bufferingProgress
    });
}

BaseVolume *PhoenixPlayBackend::baseVolume()
{
    return nullptr;
}

bool PhoenixPlayBackend::useExternalDecoder()
{
    return true;
}

bool PhoenixPlayBackend::useExternalOutPut()
{
    return true;
}

void PhoenixPlayBackend::play(quint64 startSec)
{
    if (m_engine) {
        m_engine->play ();
        m_engine->seek (startSec);
    }
}

void PhoenixPlayBackend::stop()
{
    qApp->sendPostedEvents (this, 0);
    m_url.clear ();
    if (m_engine) {
        m_engine->stop ();
        qApp->sendPostedEvents (this, 0);
    }
    m_volumeControl->reload ();

    if (m_handler->state () == PlayState::NormalError
            || m_handler->state () == PlayState::FatalError
            || m_handler->state () == PlayState::Buffering) {
        m_handler->dispatch (PlayState::Stopped); //clear error and buffering state
    }
}

void PhoenixPlayBackend::pause()
{
    if (m_engine)
        m_engine->pause ();
}

void PhoenixPlayBackend::setPosition(quint64 sec)
{
    if (m_engine)
        m_engine->seek (sec);
}

void PhoenixPlayBackend::changeMedia(BaseMediaObject *obj, quint64 startSec, bool startPlay)
{
    stop ();
    //TODO use QtMultimedia to play network stream;
    m_engine->changeMedia (obj, startSec); //don't start play, start later

    qDebug()<<Q_FUNC_INFO<<"change m_engine Media finish";

    if (m_handler->state () == PlayState::Stopped)
        m_handler->dispatch (PlayState::Buffering);

    m_engine->setMuted (m_muted);

    if (startPlay)
        m_engine->play ();
}








} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
