#include "PlayerCoreInternal.h"

#include <QDebug>

#include "AudioMetaObject.h"
#include "MediaResource.h"
#include "Backend/IPlayBackend.h"
#include "PlayerCore/MusicQueue.h"
#include "PlayerCore/RecentPlayedQueue.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

#include "PluginMgrInternal.h"
#include "MusicLibraryManagerInternal.h"
#include "PlayerCoreInternal.h"

namespace PhoenixPlayer {

PlayerCoreInternal::PlayerCoreInternal(QSharedPointer<PluginMgrInternal> mgr,
                                       QSharedPointer<MusicLibraryManagerInternal> library,
                                       QObject *parent)
    : QObject(parent),
    m_pluginMgr(mgr),
    m_library(library)
{
    m_playQueue = new MusicQueue(this);
    m_playQueue->setSizeLimit(-1);
    m_playQueue->setSkipDuplicates(false);
    connect(m_playQueue, &MusicQueue::currentIndexChanged, this, [&](int idx) {
        AudioMetaObject o = m_playQueue->get (idx);
        playTrack(o);
    });

    initPlayBackend();
}

PlayerCoreInternal::~PlayerCoreInternal()
{
    if (PluginMetaData::isValid(m_usedPlayBackend)) {
        if (m_playBackend) {
            disconnect(m_playBackend);
        }
        PluginMgr::unload(m_usedPlayBackend);
        m_playBackend = Q_NULLPTR;
    }
    if (m_playQueue) {
        m_playQueue->deleteLater();
        m_playQueue = Q_NULLPTR;
    }
}

void PlayerCoreInternal::playAt(int idx)
{
    if (idx < 0 || idx >= m_playQueue->count()) {
        qWarning()<<Q_FUNC_INFO<<"Invalid index "<<idx;
        return;
    }
    m_playQueue->setCurrentIndex(idx);
}

void PlayerCoreInternal::playTrack(const AudioMetaObject &data)
{
    if (data.isHashEmpty()) {
        qWarning()<<Q_FUNC_INFO<<"AudioMetaData is empty";
        return;
    }
    m_library->addLastPlayedTime(data);
    m_library->addPlayedCount(data);

    m_curTrack = data;
    m_curTrackDuration = data.trackMeta().duration();
    if (m_resource) {
        m_resource->deleteLater ();
        m_resource = Q_NULLPTR;
    }
    m_resource = MediaResource::create(data.uri().toString());
    qDebug()<<Q_FUNC_INFO<<"change file to "<<data.uri ();
    if (m_playBackend) {
        m_playBackend->changeMedia (m_resource, 0, true);
        m_currentPlayPos = 0;
        m_playBackend->play ();
    } else {
        qCritical()<<Q_FUNC_INFO<<"No playBackend found";
    }
    emit trackChanged(m_curTrack.toMap());
}

void PlayerCoreInternal::playFromLibrary(const QString &songHash)
{
    if (!m_playBackend) {
        qCritical()<<Q_FUNC_INFO<<"No play backend found, can't play from library";
        return;
    }
    if (!m_library->daoValid()) {
        qCritical()<<Q_FUNC_INFO<<"No music library backend found, can't play from library";
        return;
    }
    qDebug()<<Q_FUNC_INFO<<"play for hash "<<songHash;

    AudioMetaObject d = m_library->dao()->trackFromHash(songHash);

    qDebug()<<Q_FUNC_INFO<<"find in library "<<d.toMap();

    m_playQueue->addAndFocus(d);
}

void PlayerCoreInternal::playFromNetwork(const QUrl &url)
{
    if (!m_playBackend) {
        qCritical()<<Q_FUNC_INFO<<"No play backend found, can't play from library";
        return;
    }
    if (url.isEmpty() || !url.isValid()) {
        qWarning()<<Q_FUNC_INFO<<"Invalid url "<<url;
        return;
    }
    qDebug()<<Q_FUNC_INFO<<url;

    AudioMetaObject d(url);
    m_playQueue->addAndFocus(d);
}

void PlayerCoreInternal::skipForward()
{
    m_playQueue->setCurrentIndex(this->forwardIndex());
}

void PlayerCoreInternal::skipBackward()
{
    m_playQueue->setCurrentIndex(this->backwardIndex());
}

void PlayerCoreInternal::skipShuffle()
{
    m_playQueue->setCurrentIndex(this->shuffleIndex());
}

int PlayerCoreInternal::forwardIndex() const
{
    if (m_playQueue->isEmpty()) {
        return -1;
    }

    int index = m_playQueue->currentIndex() + 1;

    switch (m_playMode) {
    case PPCommon::PlayModeOrder: {
        if (index >= m_playQueue->count()) {
            index = -1;
        }
        break;
    }
    case PPCommon::PlayModeRepeatAll: {
        if (index >= m_playQueue->count()) {
            index = 0;
        }
        break;
    }
    case PPCommon::PlayModeRepeatCurrent:
        index--;
        break;
    case PPCommon::PlayModeShuffle:
        index = - 1;
        break;
    default:
        break;
    }
    qDebug()<<Q_FUNC_INFO<<" new index is "<<index;

    return index;
}

int PlayerCoreInternal::backwardIndex() const
{
    if (m_playQueue->isEmpty()) {
        return -1;
    }

    int index = m_playQueue->currentIndex() -1;

    switch (m_playMode) {
    case PPCommon::PlayModeOrder: {
        if (index < 0) {
            index = -1;
        }
        break;
    }
    case PPCommon::PlayModeRepeatAll: {
        if (index < 0) {
            index = m_playQueue->count() -1;
        }
        break;
    }
    case PPCommon::PlayModeRepeatCurrent:
        index++;
        break;
    case PPCommon::PlayModeShuffle:
        index = -1;
        break;
    default:
        break;
    }
    return index;
}

int PlayerCoreInternal::shuffleIndex() const
{
    if (m_playQueue->isEmpty()) {
        return -1;
    }
    QTime time = QTime::currentTime();
    qsrand(time.second() * 1000 + time.msec());
    int n = qrand ();
    return n % m_playQueue->count();
}

void PlayerCoreInternal::setAutoSkipForward(bool autoSkipForward)
{
    if (m_autoSkipForward != autoSkipForward) {
        m_autoSkipForward = autoSkipForward;
        emit autoSkipForwardChanged(autoSkipForward);
    }
}

void PlayerCoreInternal::setPlayMode(PPCommon::PlayMode mode)
{
    if (m_playMode != mode) {
        m_playMode = mode;
        emit playModeChanged(mode);
        emit playModeChangedInt((int)mode);
    }
}

PPCommon::PlayBackendState PlayerCoreInternal::playBackendState() const
{
    if (!m_playBackend) {
        return PPCommon::PlayBackendStopped;
    }
    return m_playBackend->playBackendState();
}

void PlayerCoreInternal::togglePlayPause()
{
    if (!m_playBackend) {
        qWarning()<<Q_FUNC_INFO<<"No play backend found !";
        return;
    }

    switch (m_playBackend->playBackendState()) {
    case PPCommon::PlayBackendPlaying:
        m_playBackend->pause();
        break;
    case PPCommon::PlayBackendPaused:
        m_playBackend->play(m_currentPlayPos);
        break;
    case PPCommon::PlayBackendStopped: {
        if (m_autoSkipForward) {
            qDebug()<<Q_FUNC_INFO<<"playbackend stopped";
            AudioMetaObject data = m_playQueue->currentTrack();
            if (data.isHashEmpty()) {
                break;
            }
            m_curTrackDuration = data.trackMeta().duration();//data->trackMeta ()->duration ();//getSongLength (data->trackMeta ()->duration ());
            m_currentPlayPos = 0;
            if (m_resource) {
                m_resource->deleteLater();
            }
            m_resource = MediaResource::create(data.uri().toString());
            m_playBackend->changeMedia(m_resource, 0, true);
        }
        break;
    }
    default:
        break;
    }
}

void PlayerCoreInternal::play()
{
    if (m_playBackend) {
        m_playBackend->play(m_currentPlayPos);
    }
}

void PlayerCoreInternal::stop()
{
    if (m_playBackend) {
        //TODO: 是否需要记住最后播放的歌曲(待后续开发情况决定)
        m_playBackend->stop();
    }
}

void PlayerCoreInternal::pause()
{
    if (m_playBackend) {
        m_playBackend->pause();
    }
}

void PlayerCoreInternal::setPosition(qreal pos, bool isPercent)
{
    if (!m_playBackend) {
        qWarning()<<Q_FUNC_INFO<<"No play backedn found !!";
        return;
    }

    qDebug()<<Q_FUNC_INFO<<"setPosition to "<<pos<<" isPercent "<<isPercent;


    m_currentPlayPos = isPercent
                           ? m_curTrackDuration <= 0 ? m_currentPlayPos : m_currentPlayPos * pos/100
                           : pos;
    m_playBackend->setPosition (m_currentPlayPos);
}

void PlayerCoreInternal::initPlayBackend()
{
    if (PluginMetaData::isValid(m_usedPlayBackend)) {
        qDebug()<<Q_FUNC_INFO<<"-------- reinit new play backend";
        if (m_playBackend) {
            m_playBackend->stop();
            disconnect(m_playBackend);
        }
        PluginMgr::unload(m_usedPlayBackend);
        m_usedPlayBackend = PluginMetaData();
        m_playBackend = Q_NULLPTR;
    }
    m_usedPlayBackend = m_pluginMgr->usedPlayBackend();
    if (!PluginMetaData::isValid(m_usedPlayBackend)) {
        qCritical()<<Q_FUNC_INFO<<"No valid playbackend found !!!";
        return;
    }
    m_playBackend = qobject_cast<PlayBackend::IPlayBackend*>(PluginMgr::instance(m_usedPlayBackend));
    if (!m_playBackend) {
        qCritical()<<Q_FUNC_INFO<<"Instance playbackedn error !!!";
        return;
    }

    // 播放状态改变信号
    connect(m_playBackend, &PlayBackend::IPlayBackend::stateChanged, [&](PPCommon::PlayBackendState state) {
        emit playBackendStateChangedInt(state);
        emit playBackendStateChanged(state);
    });

    //当一首曲目播放结束后
    connect(m_playBackend, &PlayBackend::IPlayBackend::finished, [&]() {
        emit playTrackFinished();
        if (m_autoSkipForward) {
            doPlayByPlayMode();
        }
    });

    //播放失败
    connect(m_playBackend, &PlayBackend::IPlayBackend::failed, [&]() {
        emit playTrackFailed();
        if (m_autoSkipForward) {
            doPlayByPlayMode();
        }
    });

    //tick
    connect(m_playBackend, &PlayBackend::IPlayBackend::tick, [&] (quint64 sec) {
        qDebug()<<Q_FUNC_INFO<<"Tick "<<sec<<" duration "<<m_curTrackDuration;
        m_currentPlayPos = sec;
        emit playTickActual(sec);
        if (m_curTrackDuration > 0) {
            int p = sec/m_curTrackDuration * 100;
            qDebug()<<Q_FUNC_INFO<<" Percent "<<p;
            emit playTickPercent(p);
        }
    });
    m_playBackend->initialize();
    m_playBackend->stop();
}

void PlayerCoreInternal::doPlayByPlayMode()
{
    if (!m_playBackend) {
        qDebug()<<Q_FUNC_INFO<<"Stop due to no playBackend";
        return;
    }
    qDebug()<<">>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<";
    switch (m_playMode) {
    case PPCommon::PlayModeOrder: { //顺序播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeOrder";
        if (m_playQueue->isEmpty()) {
            break;
        }
        if (m_playQueue->currentIndex() >= m_playQueue->count()-1) {
            m_playBackend->stop();
        } else {
            this->skipForward();
        }
        break;
    }
    case PPCommon::PlayModeRepeatCurrent: { //单曲播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeRepeatCurrent";

        AudioMetaObject data = m_playQueue->currentTrack();
        if (data.isHashEmpty()) {
            break;
        }
        if (m_resource) {
            m_resource->deleteLater ();
        }
        m_resource = MediaResource::create(data.uri().toString());
        m_playBackend->changeMedia(m_resource, 0, true);
        m_currentPlayPos = 0;
        m_playBackend->play();
        break;
    }
    case PPCommon::PlayModeRepeatAll:  { //循环播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeRepeatAll";
        this->skipForward();
        break;
    }
    case PPCommon::PlayModeShuffle: { //随机播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeShuffle";
        this->skipShuffle();
        break;
    }
    default:
        break;
    }
}





} //namespace PhoenixPlayer
