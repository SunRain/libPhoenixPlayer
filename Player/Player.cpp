
#include <QDebug>

#include "Player.h"

#include "Common.h"
#include "Settings.h"
#include "PlayBackendLoader.h"
#include "IPlayBackend.h"
#include "MusicLibraryManager.h"

namespace PhoenixPlayer {

Player::Player(QObject *parent) : QObject(parent)
{
    mPlayBackend = 0;
    mPlayBackendLoader = 0;
    mSettings = 0;
    mLibraryManager = 0;
}

Player::~Player()
{

}

void Player::setPlayBackendLoader(PlayBackend::PlayBackendLoader *loader)
{
    if (loader != 0) {
        mPlayBackendLoader = loader;
    }
    if (mPlayBackend.isNull ()) {
        mPlayBackend = mPlayBackendLoader->getCurrentBackend ();
    }
}

void Player::setMusicLibraryManager(MusicLibrary::MusicLibraryManager *manager)
{
    if (manager != 0) {
        mLibraryManager = manager;
    }
    connect (mLibraryManager.data (), &MusicLibrary::MusicLibraryManager::playingSongChanged, [this] () {
        QString playingHash = mLibraryManager->playingSongHash ();
        qDebug()<<"Playing song changed " << playingHash;
    });
}

void Player::setSettings(Settings *settings)
{
    if (settings !=0) {
        mSettings = settings;
    }
}

}//PhoenixPlayer
