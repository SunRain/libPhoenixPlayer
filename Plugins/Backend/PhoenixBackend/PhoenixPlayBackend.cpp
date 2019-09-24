#include "PhoenixPlayBackend.h"

#include <QDebug>
#include <QCoreApplication>

#include "StateHandler.h"
#include "InternalEvent.h"
#include "DecodeThread.h"
//#include "OutputThread_old.h"
#include "InternalEvent.h"
#include "LibPhoenixPlayerMain.h"
#include "MediaResource.h"

#include "PPCommon.h"
#include "SoftVolume.h"
#include "PlayerCore/VolumeControl.h"
#include "DecodeThread.h"
#include "OutputThread.h"
#include "Recycler.h"

//#include "DecodeThread.h"
//#include "OutputThread.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

PhoenixPlayBackend::PhoenixPlayBackend(QObject *parent)
    : IPlayBackend(parent)
{

}

PhoenixPlayBackend::~PhoenixPlayBackend()
{
    if (m_playThread) {
        m_playThread->stop();
        m_playThread->wait();

        m_playThread->deleteLater();
        m_playThread = Q_NULLPTR;
    }

    if (m_outputThread) {
        m_outputThread->stop();
        m_outputThread->wait();

        m_outputThread->deleteLater();
        m_outputThread = Q_NULLPTR;
    }

    if (m_recycler) {
        delete m_recycler;
        m_recycler = Q_NULLPTR;
    }

    if (m_handler) {
        m_handler->deleteLater();
        m_handler = Q_NULLPTR;
    }
}

bool PhoenixPlayBackend::event(QEvent *e)
{
    if (e->type() == EVENT_STATE_CHANGED) {
        PlayState st = ((StateChangedEvent *)e)->currentState();

        qDebug()<<Q_FUNC_INFO<<"Current event is EVENT_STATE_CHANGED, value is "<<playStateToName(st);

        switch (st) {
        case Playing:
            emit stateChanged(PPCommon::PlayBackendPlaying);
            break;
        case Paused:
            emit stateChanged(PPCommon::PlayBackendPaused);
            break;
        case NormalError:
        case FatalError:
            emit failed();
            break;
        default:
            emit stateChanged(PPCommon::PlayBackendStopped);
        }
    } else if (e->type () == EVENT_NEXT_TRACK_REQUEST
               || e->type () == EVENT_FINISHED) {
        emit finished();
    } else if (e->type() == EVENT_DECODE_FINISHED) {
        m_outputThread->finish();
        m_recycler->cond()->wakeAll();
    } else if (e->type() == EVENT_REQUEST_STOP_OUTPUT) {
        m_outputThread->stop();
    } else {
        return QObject::event(e);
    }
    return true;
}

PPCommon::PlayBackendState PhoenixPlayBackend::playBackendState()
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
    if (m_handler->state() == PlayState::Playing) {
        return PPCommon::PlayBackendPlaying;
    } else if (m_handler->state() == PlayState::Paused) {
        return PPCommon::PlayBackendPaused;
    } else {
        return PPCommon::PlayBackendStopped;
    }
}

void PhoenixPlayBackend::initialize()
{
    if (!m_handler) {
        m_handler = new StateHandler(this);
        connect (m_handler, &StateHandler::elapsedChanged,
                [&](qint64 time) {
                    qDebug()<<Q_FUNC_INFO<<"Tick "<<time;
                    emit tick(time/1000);
                    emit tickInMS(time);
                    emit tickInSec(time/1000);
                });

        connect (m_handler, &StateHandler::bitrateChanged,
                [&](int bitrate) {
                    //TODO bitrateChanged
                    qDebug()<<Q_FUNC_INFO<<"bitrate "<<bitrate;
                });

        connect (m_handler, &StateHandler::frequencyChanged,
                [&](quint32 frequency) {
                    //TODO frequencyChanged
                    qDebug()<<Q_FUNC_INFO<<"frequency "<<frequency;
                });

        connect (m_handler, &StateHandler::sampleSizeChanged,
                [&](int size) {
                    //TODO sampleSizeChanged
                    qDebug()<<Q_FUNC_INFO<<"sampleSizeChanged "<<size;
                });

        connect (m_handler, &StateHandler::channelsChanged,
                [&](int channels) {
                    //TODO channelsChanged
                    qDebug()<<Q_FUNC_INFO<<"channelsChanged "<<channels;
                });

        connect (m_handler, &StateHandler::bufferingProgress,
                [&](int progress) {
                    //TODO bufferingProgress
                    qDebug()<<Q_FUNC_INFO<<"bufferingProgress "<<progress;
                });
    }

    if (!m_recycler) {
        m_recycler = new Recycler();
    }

    if (!m_playThread) {
        m_playThread = new DecodeThread(m_handler, m_recycler, this);
    }

    if (!m_outputThread) {
        m_outputThread = new OutputThread(m_handler, m_recycler, this);
    }
    m_outputThread->setMuted(m_muted);

    if (!m_volumeControl) {
        m_volumeControl = phoenixPlayerLib->volumeCtrl();

        connect (m_volumeControl, &VolumeControl::mutedChanged,
                [&] (bool muted) {
                    m_muted = muted;
                    m_outputThread->setMuted(muted);
                });
    }
}

BaseVolume *PhoenixPlayBackend::baseVolume()
{
    if (!m_volume) {
        m_volume = SoftVolume::instance();
    }
    return m_volume;
}

bool PhoenixPlayBackend::useExternalDecoder()
{
    return false;
}

bool PhoenixPlayBackend::useExternalOutPut()
{
    return false;
}

qint64 PhoenixPlayBackend::durationInMS()
{
    if (!m_handler) {
        return  -1;
    }
    return m_handler->durationInMS();
}

void PhoenixPlayBackend::play(quint64 startSec)
{
    qDebug()<<Q_FUNC_INFO<<"play with startSec "<<startSec;
    m_playThread->seek(startSec * 1000);
    m_outputThread->seek(startSec * 1000, true);

    if (!m_playThread->isRunning()) {
        m_playThread->play();
    }
    if (!m_outputThread->isRunning()) {
        m_outputThread->start();
    }
    if (m_outputThread->isRunning() && m_outputThread->isPaused()) {
        m_outputThread->togglePlayPause();
    }
}

void PhoenixPlayBackend::stop()
{
    qDebug()<<Q_FUNC_INFO<<"############### ";

    m_playThread->stop();
    m_outputThread->stop();

    m_volumeControl->reload ();

    if (m_handler->state () == PlayState::NormalError
            || m_handler->state () == PlayState::FatalError
            || m_handler->state () == PlayState::Buffering) {
        m_handler->dispatch (PlayState::Stopped); //clear error and buffering state
    }
}

void PhoenixPlayBackend::pause()
{
    if (!m_outputThread->isPaused()) {
        m_outputThread->togglePlayPause();
    }
}

void PhoenixPlayBackend::setPosition(quint64 sec)
{
    qDebug()<<Q_FUNC_INFO<<" setPosition "<<sec;
    m_playThread->seek(sec * 1000);
    m_outputThread->seek(sec * 1000);
}

void PhoenixPlayBackend::changeMedia(MediaResource *res, quint64 startSec, bool startPlay)
{
    this->stop();

    if (!res) {
        qWarning()<<Q_FUNC_INFO<<"No media resource";
        return;
    }
    if (m_resource) {
        m_resource->deleteLater();
        m_resource = Q_NULLPTR;
    }
    //TODO post state to backend
    if (!res->initialize()) {
        res->deleteLater();
        qWarning()<<Q_FUNC_INFO<<"initialize resource error !";
        return;
    }
    m_resource = res;
    m_resource->setParent(this);

    if (!m_playThread->initialization(res, startSec)) {
        qWarning()<<Q_FUNC_INFO<<"initialize decode thread error";
        return;
    }
    if (!m_outputThread->initialization(m_playThread->audioParameters().sampleRate(),
                                    m_playThread->audioParameters().channelMap())) {
        qWarning()<<Q_FUNC_INFO<<"initialize output thread error";
        return;
    }
    m_outputThread->setMuted(m_muted);

    if (startPlay) {
        this->play(startSec);
    }
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
