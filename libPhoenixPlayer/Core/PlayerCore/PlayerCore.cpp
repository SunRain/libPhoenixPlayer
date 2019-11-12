#include "PlayerCore/PlayerCore.h"

#include <QDebug>
#include <QUrl>

#include "private/PlayerCoreInternal.h"
#include "private/SingletonObjectFactory.h"

namespace PhoenixPlayer {

PlayerCore::PlayerCore(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->playerCoreInternal();

    connect(m_internal.data(), &PlayerCoreInternal::trackChanged,
            this, &PlayerCore::trackChanged);

    connect(m_internal.data(), &PlayerCoreInternal::playTrackFinished,
            this, &PlayerCore::playTrackFinished);

    connect(m_internal.data(), &PlayerCoreInternal::playTrackFailed,
            this, &PlayerCore::playTrackFailed);

    connect(m_internal.data(), &PlayerCoreInternal::playTickActual,
            this, &PlayerCore::playTickActual);

    connect(m_internal.data(), &PlayerCoreInternal::playTickPercent,
            this, &PlayerCore::playTickPercent);

    connect(m_internal.data(), &PlayerCoreInternal::playModeChanged,
            this, &PlayerCore::playModeChanged);

    connect(m_internal.data(), &PlayerCoreInternal::playModeChangedInt,
            this, &PlayerCore::playModeChangedInt);

    connect(m_internal.data(), &PlayerCoreInternal::playBackendStateChangedInt,
            this, &PlayerCore::playBackendStateChangedInt);

    connect(m_internal.data(), &PlayerCoreInternal::playBackendStateChanged,
            this, &PlayerCore::playBackendStateChanged);

    connect(m_internal.data(), &PlayerCoreInternal::autoSkipForwardChanged,
            this, &PlayerCore::autoSkipForwardChanged);

}

PlayerCore::~PlayerCore()
{
    m_internal->disconnect(this);
}

PPCommon::PlayMode PlayerCore::playMode() const
{
    return m_internal->playMode();
}

int PlayerCore::playModeInt() const
{
    return (int)playMode();
}

PPCommon::PlayBackendState PlayerCore::playBackendState() const
{
    return m_internal->playBackendState();
}

int PlayerCore::playBackendStateInt() const
{
    return (int)playBackendState();
}

MusicQueue *PlayerCore::playQueue() const
{
    return m_internal->playQueue();
}

AudioMetaObject PlayerCore::curTrackMetaObject() const
{
    return m_internal->curTrackMetaObject();
}

QVariantMap PlayerCore::currentTrack() const
{
    return m_internal->curTrackMetaObject().toMap();
}

bool PlayerCore::autoSkipForward() const
{
    return m_internal->autoSkipForward();
}

void PlayerCore::playAt(int idx)
{
    m_internal->playAt(idx);
}

void PlayerCore::playFromLibrary(const QString &songHash)
{
    m_internal->playFromLibrary(songHash);
}

void PlayerCore::playFromNetwork(const QUrl &url)
{
    m_internal->playFromNetwork(url);
}

void PlayerCore::playTrack(const AudioMetaObject &data)
{
    m_internal->playTrack(data);
}

int PlayerCore::forwardIndex() const
{
    return m_internal->forwardIndex();
}

int PlayerCore::backwardIndex() const
{
    return m_internal->backwardIndex();
}

int PlayerCore::shuffleIndex() const
{
    return m_internal->shuffleIndex();
}

void PlayerCore::togglePlayPause()
{
    m_internal->togglePlayPause();
}

void PlayerCore::play()
{
    m_internal->play();
}

void PlayerCore::stop()
{
    m_internal->stop();
}

void PlayerCore::pause()
{
    m_internal->pause();
}

void PlayerCore::setPosition(qreal pos, bool isPercent)
{
    m_internal->setPosition(pos, isPercent);
}

void PlayerCore::skipForward()
{
    m_internal->skipForward();
}

void PlayerCore::skipBackward()
{
    m_internal->skipBackward();
}

void PlayerCore::skipShuffle()
{
    m_internal->skipShuffle();
}
void PlayerCore::setPlayMode(PPCommon::PlayMode mode)
{
    m_internal->setPlayMode(mode);
}

void PlayerCore::setPlayModeInt(int mode)
{
    setPlayMode(PPCommon::PlayMode(mode));
}

void PlayerCore::setAutoSkipForward(bool autoSkipForward)
{
    m_internal->setAutoSkipForward(autoSkipForward);
}

}//PhoenixPlayer
