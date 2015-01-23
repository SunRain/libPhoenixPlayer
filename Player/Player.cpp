
#include <QDebug>

#include "Player.h"

#include "Common.h"
#include "Settings.h"
#include "PlayBackendLoader.h"
#include "IPlayBackend.h"
#include "MusicLibraryManager.h"
#include "BaseMediaObject.h"

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
        qDebug()<<"user playbackend "<<mPlayBackend->getBackendName ();
        mPlayBackend->init ();
        mPlayBackend->stop ();
    }
    connect (mPlayBackendLoader.data (), &PlayBackend::PlayBackendLoader::signalPlayBackendChanged, [this] {
        mPlayBackend = mPlayBackendLoader->getCurrentBackend ();
        qDebug()<<"change playbackend to"<<mPlayBackend->getBackendName ();
        mPlayBackend->init ();
        mPlayBackend->stop ();
    });
}

void Player::setMusicLibraryManager(MusicLibrary::MusicLibraryManager *manager)
{
    if (manager != 0) {
        mLibraryManager = manager;
    }
    connect (mLibraryManager.data (), &MusicLibrary::MusicLibraryManager::playingSongChanged, [this] () {
        QString playingHash = mLibraryManager->playingSongHash ();
        qDebug()<<"****** playingSongChanged **********";
        if (!mPlayBackend.isNull ()) {
            PlayBackend::BaseMediaObject obj;
            obj.setFileName (mLibraryManager->querySongMetaElement (Common::E_FileName, playingHash).first ());
            obj.setFilePath (mLibraryManager->querySongMetaElement (Common::E_FilePath, playingHash).first ());
            obj.setMediaType (Common::TypeLocalFile);
            qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
            mPlayBackend->changeMedia (&obj, 0, true);
        }
    });
}

void Player::setSettings(Settings *settings)
{
    if (settings !=0) {
        mSettings = settings;
    }
}

}//PhoenixPlayer
