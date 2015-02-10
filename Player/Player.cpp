
#include <QDebug>

#include "Player.h"

#include "Common.h"
#include "Settings.h"
#include "MusicLibraryManager.h"
#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MetadataLookupManager.h"
#include "SongMetaData.h"
#include "MusicLibrary/IPlayListDAO.h"

#include "SingletonPointer.h"

namespace PhoenixPlayer {

using namespace MetadataLookup;
using namespace MusicLibrary;
using namespace PlayBackend;

Player::Player(QObject *parent)
    : QObject(parent)
    ,isInit(false)
{
    mPlayBackend = 0;

#ifdef SAILFISH_OS
    mPluginLoader = PluginLoader::instance();
    mSettings = Settings::instance();
    mMusicLibraryManager = MusicLibraryManager::instance();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
    mSettings = SingletonPointer<Settings>::instance ();
    mMusicLibraryManager = SingletonPointer<MusicLibraryManager>::instance ();
#endif

    mMetaLookupManager = new MetadataLookupManager(this);

    mPlayMode = Common::PlayModeOrder;

    if (!isInit)
        init ();
}

Player::~Player()
{
    qDebug()<<"[Player]"<<__FUNCTION__;
    if (mMetaLookupManager)
        delete mMetaLookupManager;
    qDebug()<<"[Player] after "<<__FUNCTION__;
}

void Player::setPluginLoader()
{
    if (mPluginLoader == nullptr) {
        qDebug()<<"[Player] No mPluginLoader";
        return;
    }
    if (mPlayBackend.isNull ()) {
        mPlayBackend = mPluginLoader->getCurrentPlayBackend ();
        if (!PointerValid (EPointer::PPlaybackend))
            return;
        qDebug()<<"[Player] user playbackend "<<mPlayBackend->getBackendName ();
        mPlayBackend->init ();
        mPlayBackend->stop ();
    }
    connect (mPluginLoader,
             &PluginLoader::signalPluginChanged,
             [this](PluginLoader::PluginType type) {
        if (type == PluginLoader::TypePlayBackend) {
            if (mPlayBackend) {
                mPlayBackend->stop ();
                mPlayBackend.data ()->deleteLater ();
            }
            mPlayBackend = mPluginLoader->getCurrentPlayBackend ();
            if (!PointerValid (EPointer::PPlaybackend))
                return;
            qDebug()<<"change playbackend to"<<mPlayBackend->getBackendName ();
            mPlayBackend->init ();
            mPlayBackend->stop ();
        }
    });

    if (!PointerValid (EPointer::PPlaybackend)) {
        qDebug()<<"[Player] No Playbackend found";
        return;
    }

    // 播放状态改变信号
    connect (mPlayBackend.data (),
             &IPlayBackend::stateChanged,
             [this](Common::PlaybackState state) {
        emit playStateChanged (state);
        emit playStateChanged ((int)state);
    });

    //当一首曲目播放结束后
    connect (mPlayBackend.data (),
             &IPlayBackend::finished,
             [this] {
        if (PointerValid (EPointer::PMusicLibraryManager)) {
            switch (mPlayMode) {
            case Common::PlayModeOrder: { //顺序播放
                if (mMusicLibraryManager->lastSongHash ()
                        == mMusicLibraryManager->playingSongHash ()) {
                    mPlayBackend.data ()->stop ();
                } else {
                    mMusicLibraryManager->nextSong ();
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
                mMusicLibraryManager->nextSong ();
                break;
            }
            case Common::PlayModeShuffle: { //随机播放
                mMusicLibraryManager->randomSong ();
                break;
            }
            default:
                break;
            }
        }
    });

    //播放失败
    connect (mPlayBackend.data (),
             &IPlayBackend::failed,
             [this]() {
        if (PointerValid (EPointer::PMusicLibraryManager)) {
            mMusicLibraryManager->nextSong ();
        }
    });

    //tick
    connect (mPlayBackend.data (),
             &IPlayBackend::tick,
             [this] (quint64 sec) {
        mCurrentPlayPos = sec;
        emit playTickActual (sec);
        if (mCurrentSongLength <= 0)
            return;

        emit playTickPercent (((qreal)sec/mCurrentSongLength) * 100);
    });
}

void Player::setMusicLibraryManager()
{
    if (mMusicLibraryManager == nullptr) {
        qDebug()<<"[Player] no MusicLibraryManager";
        return;
    }
    //播放列表上一首/下一首/随机
    connect (mMusicLibraryManager,
             &MusicLibraryManager::playingSongChanged, [this] () {
        if (!PointerValid (EPointer::PPlaybackend)) {
            qDebug()<<"[Player] Can't connect playingSongChanged ";
            return;
        }
        QString playingHash = mMusicLibraryManager->playingSongHash ();

        mCurrentSongLength = getSongLength (playingHash);

        qDebug()<<"Player playingSongChanged mCurrentSongLength"
               <<mCurrentSongLength;

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
    });
}

void Player::setMetaLookupManager()
{
    if (mMetaLookupManager == nullptr) {
        qDebug()<<"[Player] no MetaLookupManager";
        return;
    }
    connect (mMetaLookupManager,
             &MetadataLookupManager::lookupFailed,
             [this] {
        emitMetadataLookupResult (IMetadataLookup::TypeUndefined, false);
    });

    connect (mMetaLookupManager,
             &MetadataLookupManager::lookupSucceed,
             [this]
             (QString songHash,
             QByteArray result,
             IMetadataLookup::LookupType type) {

        if (type == IMetadataLookup::TypeLyrics) {
            //将lyric数据写入数据库中
            SongMetaData meta;
            meta.setMeta (Common::SongMetaTags::E_Hash, songHash);
            meta.setMeta (Common::SongMetaTags::E_Lyrics, result);

            //TODO 也许通过MusicLibraryManager来管理会更好
            if (PointerValid (EPointer::PPluginLoader)) {
                MusicLibrary::IPlayListDAO *dao =
                        mPluginLoader->getCurrentPlayListDAO ();
                if (dao)
                    dao->updateMetaData (&meta, true);
            }
        }
        emitMetadataLookupResult (type, true);
    });
}

//void Player::setSettings()
//{
//    mSettings = SingletonPointer<Settings>::getInstance ();
//}

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

void Player::lookupLyric(const QString &songHash)
{
    metadataLookup (songHash, IMetadataLookup::TypeLyrics);
}

void Player::togglePlayPause()
{
    if (!PointerValid (EPointer::PPlaybackend))
        return;

    switch (mPlayBackend.data ()->getPlaybackState ()) {
    case Common::PlaybackPlaying:
        mPlayBackend.data ()->pause ();
        break;
    case Common::PlaybackPaused:
        mPlayBackend.data ()->play (mCurrentPlayPos);
        break;
    case Common::PlaybackStopped: {
        if (PointerValid (EPointer::PMusicLibraryManager)) {
            QString playingHash = mMusicLibraryManager->playingSongHash ();

            mCurrentSongLength = getSongLength (playingHash);
            mCurrentPlayPos = 0;

            //设置播放歌曲的hash,使得MusicLibraryManager发送playingSongChanged信号
            //此处是为了使得前端qml界面能够在初始化时候刷新
            mMusicLibraryManager->setPlayingSongHash (playingHash);

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
        break;
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

    qDebug()<<"Player setPosition to "<<pos<<" isPercent "<<isPercent;

    if (isPercent) {
        if (mCurrentSongLength <= 0)
            return;
        mPlayBackend.data ()->setPosition (mCurrentSongLength * pos/100);
    } else {
        mPlayBackend.data ()->setPosition (pos);
    }
}

void Player::init()
{
    setPluginLoader();
    setMusicLibraryManager();
    setMetaLookupManager ();
    isInit = true;

    qDebug()<<"[Player] after "<<__FUNCTION__;
}

bool Player::PointerValid(Player::EPointer pointer)
{
    bool valid = false;
    switch (pointer) {
    case EPointer::PPlaybackend:
        valid = !mPlayBackend.isNull ();
        break;
    case EPointer::PPluginLoader:
        valid = (mPluginLoader != nullptr);
        break;
    case EPointer::PMusicLibraryManager:
        valid = (mMusicLibraryManager != nullptr);
        break;
    case EPointer::PPLyricsManager:
        valid = (mMetaLookupManager != nullptr);
        break;
    default:
        break;
    }
    return valid;
}

int Player::getSongLength(const QString &hash)
{
    if (hash.isEmpty ())
        return 0;

    if (!PointerValid (EPointer::PMusicLibraryManager))
        return 0;

    QStringList list = mMusicLibraryManager
            ->querySongMetaElement (Common::SongMetaTags::E_SongLength,
                                    hash, true);
    if (!list.isEmpty ()) {
       return list.first ().toULongLong ();
    }
    return 0;
}

void Player::metadataLookup(const QString &songHash,
                            IMetadataLookup::LookupType type)
{
    SongMetaData data;
    QString hash = songHash;
    if (hash.isEmpty ())
        hash = mMusicLibraryManager->playingSongHash ();

    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag;
         ++i) {
        QStringList list = mMusicLibraryManager
                ->querySongMetaElement (Common::SongMetaTags(i), hash, true);
        if (list.isEmpty ())
            data.setMeta (Common::SongMetaTags(i), QVariant());
        else
            data.setMeta (Common::SongMetaTags(i), list.first ());
    }
    mMetaLookupManager->lookup (&data, type);
}

void Player::emitMetadataLookupResult(
        IMetadataLookup::LookupType type, bool result)
{
    if (!result) {
        emit metadataLookupFailed ();
        return;
    }
    //TODO 添加其他类型的emit
    switch (type) {
    case IMetadataLookup::TypeLyrics:
        lookupLyricSucceed ();
        break;
    default:
        break;
    }
}















}//PhoenixPlayer
