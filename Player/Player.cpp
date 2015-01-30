
#include <QDebug>

#include "Player.h"

#include "Common.h"
#include "Settings.h"
#include "MusicLibraryManager.h"
#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"

namespace PhoenixPlayer {

Player::Player(QObject *parent) : QObject(parent)
{
    mPlayBackend = 0;
    //    mPlayBackendLoader = 0;
    mPluginLoader = 0;
    mSettings = 0;
    mMusicLibraryManager = 0;
    mPlayMode = Common::PlayModeOrder;
}

Player::~Player()
{

}

void Player::setPluginLoader(PluginLoader *loader)
{
    if (loader != 0) {
        mPluginLoader = loader;
    }

    if (mPlayBackend.isNull ()) {
        mPlayBackend = mPluginLoader.data ()->getCurrentPlayBackend ();
        if (!PointerValid (EPointer::PPlaybackend))
            return;
        qDebug()<<"user playbackend "<<mPlayBackend->getBackendName ();
        mPlayBackend->init ();
        mPlayBackend->stop ();
    }
    connect (mPluginLoader.data (),
             &PluginLoader::signalPluginChanged,
             [this](PluginLoader::PluginType type) {
        if (type == PluginLoader::TypePlayBackend) {
            mPlayBackend->stop ();
            mPlayBackend = mPluginLoader.data ()->getCurrentPlayBackend ();
            if (!PointerValid (EPointer::PPlaybackend))
                return;
            qDebug()<<"change playbackend to"<<mPlayBackend->getBackendName ();
            mPlayBackend->init ();
            mPlayBackend->stop ();
        }
    });

    if (PointerValid (EPointer::PPlaybackend)) {
        // 播放状态改变信号
        connect (mPlayBackend.data (),
                 &PlayBackend::IPlayBackend::stateChanged,
                 [this](Common::PlaybackState state) {
            emit playStateChanged (state);
            emit playStateChanged ((int)state);
        });

        //当一首曲目播放结束后
        connect (mPlayBackend.data (), &PlayBackend::IPlayBackend::finished, [this] {
            if (PointerValid (EPointer::PMusicLibraryManager)) {
                switch (mPlayMode) {
                case Common::PlayModeOrder: { //顺序播放
                    if (mMusicLibraryManager.data ()->lastSongHash ()
                            == mMusicLibraryManager.data ()->playingSongHash ()) {
                        mPlayBackend.data ()->stop ();
                    } else {
                        mMusicLibraryManager.data ()->nextSong ();
                    }
                    break;
                }
                case Common::PlayModeRepeatCurrent: { //单曲播放
                    QString playingHash = mMusicLibraryManager->playingSongHash ();
                    PlayBackend::BaseMediaObject obj;

                    QStringList list = mMusicLibraryManager
                            ->querySongMetaElement (Common::E_FileName, playingHash);
                    if (!list.isEmpty ())
                        obj.setFileName (list.first ());

                    list = mMusicLibraryManager
                            ->querySongMetaElement (Common::E_FilePath, playingHash);
                    if (!list.isEmpty ())
                        obj.setFilePath (list.first ());

                    obj.setMediaType (Common::MediaTypeLocalFile);
                    mPlayBackend.data ()->changeMedia (&obj, 0, true);
                    break;
                }
                case Common::PlayModeRepeatAll:  { //循环播放
                    mMusicLibraryManager.data ()->nextSong ();
                    break;
                }
                case Common::PlayModeShuffle: { //随机播放
                    mMusicLibraryManager.data ()->randomSong ();
                    break;
                }
                default:
                    break;
                }
            }
        });
    }

}

void Player::setMusicLibraryManager(MusicLibrary::MusicLibraryManager *manager)
{
    if (manager == 0)
        return;
    mMusicLibraryManager = manager;

    //播放列表上一首/下一首/随机
    connect (mMusicLibraryManager.data (),
             &MusicLibrary::MusicLibraryManager::playingSongChanged, [this] () {
        if (PointerValid (EPointer::PPlaybackend)) {
            QString playingHash = mMusicLibraryManager->playingSongHash ();
            PlayBackend::BaseMediaObject obj;

            QStringList list = mMusicLibraryManager
                    ->querySongMetaElement (Common::E_FileName, playingHash);
            if (!list.isEmpty ())
                obj.setFileName (list.first ());

            list = mMusicLibraryManager
                    ->querySongMetaElement (Common::E_FilePath, playingHash);
            if (!list.isEmpty ())
                obj.setFilePath (list.first ());

            obj.setMediaType (Common::MediaTypeLocalFile);
            qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
            mPlayBackend.data ()->changeMedia (&obj, 0, true);
        }
    });
}

void Player::setSettings(Settings *settings)
{
    if (settings !=0) {
        mSettings = settings;
    }
}

void Player::setPlayMode(Common::PlayMode mode)
{
    mPlayMode = mode;
    qDebug()<<"Play mode changed to "<<mPlayMode;
    emit playModeChanged (mode);
    emit playModeChanged (int(mode));
}

void Player::setPlayModeInt(int mode)
{
    setPlayMode (Common::PlayMode(mode));
}

Common::PlayMode Player::getPlayMode()
{
    return mPlayMode;
}

void Player::togglePlayPause()
{
    if (!PointerValid (EPointer::PPlaybackend))
        return;

    //    qDebug()<<__FUNCTION__ <<" current "<<mPlayBackend.data ()->getPlaybackState ();

    switch (mPlayBackend.data ()->getPlaybackState ()) {
    case Common::PlaybackPlaying:
        mPlayBackend.data ()->pause ();
        break;
    case Common::PlaybackPaused:
        //TODO: 播放中途暂停时候需要设置当前位置开始播放
        mPlayBackend.data ()->play ();
    case Common::PlaybackStopped: {
        if (PointerValid (EPointer::PMusicLibraryManager)) {
            QString playingHash = mMusicLibraryManager->playingSongHash ();
            PlayBackend::BaseMediaObject obj;
            QStringList list = mMusicLibraryManager
                    ->querySongMetaElement (Common::E_FileName, playingHash);
            if (!list.isEmpty ())
                obj.setFileName (list.first ());
            list = mMusicLibraryManager
                    ->querySongMetaElement (Common::E_FilePath, playingHash);
            if (!list.isEmpty ())
                obj.setFilePath (list.first ());
            obj.setMediaType (Common::MediaTypeLocalFile);
            qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
            mPlayBackend.data ()->changeMedia (&obj, 0, true);
        }
    }
    default:
        break;
    }

}

void Player::play()
{
    if (PointerValid (EPointer::PPlaybackend))
        //TODO: 播放中途暂停时候需要设置当前位置开始播放
        mPlayBackend.data ()->play ();
}

void Player::stop()
{
    if (PointerValid (EPointer::PPlaybackend))
        //TODO: 是否需要记住最后播放的歌曲(待后续开发情况决定)
        mPlayBackend.data ()->stop ();
}

void Player::pause()
{
    if (PointerValid (EPointer::PPlaybackend))
        mPlayBackend.data ()->pause ();
}

void Player::setVolume(int vol)
{
    if (PointerValid (EPointer::PPlaybackend))
        mPlayBackend.data ()->setVolume (vol);
}

void Player::setPosition(qreal pos, bool isPercent)
{
    if (!PointerValid (EPointer::PPlaybackend))
        return;

    if (isPercent) {
        //TODO: 百分比跳转情况
    } else {
        mPlayBackend.data ()->setPosition (pos);
    }
}

bool Player::PointerValid(Player::EPointer pointer)
{
    bool valid = false;
    switch (pointer) {
    case EPointer::PPlaybackend:
        valid = !mPlayBackend.isNull ();
        break;
    case EPointer::PPluginLoader:
        valid =!mPluginLoader.isNull ();
    case EPointer::PMusicLibraryManager:
        valid = !mMusicLibraryManager.isNull ();
    default:
        break;
    }
    return valid;
}















}//PhoenixPlayer
