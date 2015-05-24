#include "Player/Player.h"

#include <QDebug>
#include <QMutex>
#include <QScopedPointer>

#include "Common.h"
#include "Settings.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MetadataLookup/MetadataLookupManager.h"
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

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
    mSettings = Settings::instance();
    mMusicLibraryManager = MusicLibraryManager::instance();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
    mSettings = SingletonPointer<Settings>::instance ();
    mMusicLibraryManager = SingletonPointer<MusicLibraryManager>::instance ();
#endif

    mMetaLookupManager = nullptr;//new MetadataLookupManager(this);
    mPlayQueue = QStringList();
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

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
Player *Player::instance()
{
    static QMutex mutex;
    static QScopedPointer<Player> scp;
    if (Q_UNLIKELY(scp.isNull())) {
        qDebug()<<Q_FUNC_INFO<<">>>>>>>> statr new";
        mutex.lock();
        scp.reset(new Player(0));
        mutex.unlock();
    }
    return scp.data();
}
#endif

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
//        qDebug()<<"[Player] user playbackend "<<mPlayBackend->getPluginName ();
        mPlayBackend->init ();
        mPlayBackend->stop ();
    }
    connect (mPluginLoader,
             &PluginLoader::signalPluginChanged,
             [this](Common::PluginType type) {
        if (type == Common::PluginPlayBackend) {
            if (mPlayBackend) {
                mPlayBackend->stop ();
                mPlayBackend.data ()->deleteLater ();
            }
            mPlayBackend = mPluginLoader->getCurrentPlayBackend ();
            if (!PointerValid (EPointer::PPlaybackend))
                return;
//            qDebug()<<"change playbackend to"<<mPlayBackend->getPluginName ();
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
             [this](Common::PlayBackendState state) {
        emit playBackendStateChanged (state);
        emit playBackendStateChanged ((int)state);
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
        if (!mPlayQueue.contains (playingHash))
            mPlayQueue.append (playingHash);

        PlayBackend::BaseMediaObject obj;
        obj.setFileName (mMusicLibraryManager->queryOne(playingHash, Common::E_FileName));
        obj.setFilePath (mMusicLibraryManager->queryOne(playingHash, Common::E_FilePath));
        obj.setMediaType (Common::MediaTypeLocalFile);
        qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
        mPlayBackend.data ()->changeMedia (&obj, 0, true);
    });
}

void Player::setMetaLookupManager()
{
    if (mMetaLookupManager == nullptr) {
        mMetaLookupManager = new MetadataLookupManager(this);
        connect (mMetaLookupManager,
                 &MetadataLookupManager::queueFinished,
                 [this] {
            qDebug()<<"========>>> queueFinished <<<<========";
            delete mMetaLookupManager;
            mMetaLookupManager = nullptr;
        });

        connect (mMetaLookupManager,
                 &MetadataLookupManager::lookupFailed,
                 [this] (const QString &songHash, IMetadataLookup::LookupType type){
            emitMetadataLookupResult (type, songHash, false);
        });

        connect (mMetaLookupManager,
                 &MetadataLookupManager::lookupSucceed,
                 [this]
                 (QString songHash,
                 QByteArray result,
                 IMetadataLookup::LookupType type) {
            SongMetaData meta;
            meta.setMeta (Common::SongMetaTags::E_Hash, songHash);
            switch (type) {
            case IMetadataLookup::TypeAlbumDate:
                meta.setMeta (Common::E_AlbumYear, result);
                break;
            case IMetadataLookup::TypeAlbumDescription:
                meta.setMeta (Common::E_AlbumDescription, result);
                break;
            case IMetadataLookup::TypeAlbumImage:
                meta.setMeta (Common::E_AlbumImageUrl, result);
                break;
            case  IMetadataLookup::TypeArtistDescription:
                meta.setMeta (Common::E_ArtistDescription, result);
                break;
            case IMetadataLookup::TypeArtistImage:
                meta.setMeta (Common::E_ArtistImageUri, result);
                break;
            case IMetadataLookup::TypeLyrics:
                meta.setMeta (Common::E_Lyrics, result);
                break;
            case IMetadataLookup::TypeTrackDescription:
                meta.setMeta (Common::E_SongDescription, result);
                break;
            default:
                break;
            }
            //TODO 也许通过MusicLibraryManager来管理会更好
            if (PointerValid (EPointer::PPluginLoader)) {
                MusicLibrary::IPlayListDAO *dao =
                        mPluginLoader->getCurrentPlayListDAO ();
                if (dao)
                    dao->updateMetaData (&meta, true);
            }
            emitMetadataLookupResult (type, songHash, true);
        });
    }
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

void Player::setPlayMode(int mode)
{
    setPlayMode (Common::PlayMode(mode));
}

Common::PlayMode Player::getPlayMode()
{
    return mPlayMode;
}

int Player::getPlayModeInt()
{
    return (int)getPlayMode();
}


Common::PlayBackendState Player::getPlayBackendState()
{
    if (!PointerValid((EPointer::PPlaybackend))) {
        Common::PlayBackendState s = Common::PlayBackendStopped;
        return s;
    }
    return mPlayBackend.data()->getPlayBackendState();
}

int Player::getPlayBackendStateInt()
{
    return (int)getPlayBackendState();
}

void Player::playFromLibrary(const QString &songHash)
{
    mMusicLibraryManager->setPlayingSongHash (songHash);
}

void Player::addToQueue(const QString &songHash)
{
    if (!songHash.isEmpty ())
        mPlayQueue.append (songHash);
}

QStringList Player::getPlayQueue()
{
    return mPlayQueue;
}

bool Player::removeQueueItemAt(int index)
{
    if (index < 0)
        return false;
    if (index > mPlayQueue.length () -1)
        return false;
    if (mPlayQueue.isEmpty ())
        return false;
    mPlayQueue.removeAt (index);
    return true;
}

QString Player::forwardTrackHash()
{
    if (mMusicLibraryManager->getCurrentPlayListHash ().isEmpty () && !mPlayQueue.isEmpty ()) {
        int index = mPlayQueue.indexOf (mMusicLibraryManager->playingSongHash ()) +1;
        if (index >= mPlayQueue.size ())
            index = 0;
        return mPlayQueue.at (index);
    }
    return mMusicLibraryManager->nextSong (false);
}

QString Player::backwardTrackHash()
{
    if (mMusicLibraryManager->getCurrentPlayListHash ().isEmpty () && !mPlayQueue.isEmpty ()) {
        int index = mPlayQueue.indexOf (mMusicLibraryManager->playingSongHash ());
        if (index == -1) { //no hash found
            index = 0;
        } else if (index == 0) { //hash is the first song
            index = mPlayQueue.size () -1; //jump to last song
        } else {
            index --;
        }
        return mPlayQueue.at (index);
    }
    return mMusicLibraryManager->preSong (false);
}

void Player::lookupLyric(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeLyrics);
}

void Player::lookupAlbumImage(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeAlbumImage);
}

void Player::lookupAlbumDescription(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeAlbumDescription);
}

void Player::lookupAlbumDate(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeAlbumDate);
}

void Player::lookupArtistImage(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeArtistImage);
}

void Player::lookupArtistDescription(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeArtistDescription);
}

void Player::lookupTrackDescription(const QString &songHash)
{
    doMetadataLookup (songHash, IMetadataLookup::TypeTrackDescription);
}

void Player::togglePlayPause()
{
    if (!PointerValid (EPointer::PPlaybackend))
        return;

    switch (mPlayBackend.data ()->getPlayBackendState ()) {
    case Common::PlayBackendPlaying:
        mPlayBackend.data ()->pause ();
        break;
    case Common::PlayBackendPaused:
        mPlayBackend.data ()->play (mCurrentPlayPos);
        break;
    case Common::PlayBackendStopped: {
        if (PointerValid (EPointer::PMusicLibraryManager)) {
            qDebug()<<__FUNCTION__<<"playbackend stopped";
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

void Player::skipForward()
{
    this->playFromLibrary (this->forwardTrackHash ());
}

void Player::skipBackward()
{
    this->playFromLibrary (this->backwardTrackHash ());
}

void Player::skipShuffle()
{
    if (mMusicLibraryManager->getCurrentPlayListHash ().isEmpty () && !mPlayQueue.isEmpty ()) {
        QTime time = QTime::currentTime ();
        qsrand(time.second () * 1000 + time.msec ());
        int n = qrand ();
        n = n % mPlayQueue.size ();
        this->playFromLibrary (mPlayQueue.at (n));
    } else {
        mMusicLibraryManager->randomSong ();
    }
}

void Player::init()
{
    setPluginLoader();
    setMusicLibraryManager();
//    setMetaLookupManager ();
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

void Player::doMetadataLookup(const QString &songHash,
                            IMetadataLookup::LookupType type)
{
    setMetaLookupManager ();

    SongMetaData data;
    QString hash = songHash;
    if (hash.isEmpty ()) {
        qDebug()<<__FUNCTION__<<"hash is empty";
        hash = mMusicLibraryManager->playingSongHash ();
    }

    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag;
         ++i) {
        QString str = mMusicLibraryManager->queryOne(hash, Common::SongMetaTags(i), true);
        if (str.isEmpty())
            data.setMeta (Common::SongMetaTags(i), QVariant());
        else
            data.setMeta (Common::SongMetaTags(i), str);

    }
    mMetaLookupManager->lookup (&data, type);
}

void Player::emitMetadataLookupResult(IMetadataLookup::LookupType type, const QString &hash, bool succeed)
{
    //TODO 添加其他类型的emit
    switch (type) {
    case IMetadataLookup::TypeLyrics: {
        if (succeed)
            emit lookupLyricSucceed (hash);
        else
            emit lookupLyricFailed (hash);
        break;
    }
    case IMetadataLookup::TypeAlbumDescription: {
        if (succeed)
            emit lookupAlbumDescriptionSucceed (hash);
        else
            emit lookupAlbumDescriptionFailed (hash);
        break;
    }
    case IMetadataLookup::TypeAlbumImage: {
        if (succeed)
            emit lookupAlbumImageSucceed (hash);
        else
            emit lookupAlbumImageFailed (hash);
        break;
    }
    case IMetadataLookup::TypeAlbumDate: {
        if (succeed)
            emit lookupAlbumDateSucceed (hash);
        else
            emit lookupAlbumDateFailed (hash);
        break;
    }
    case IMetadataLookup::TypeArtistDescription: {
        if (succeed)
            emit lookupArtistDescriptionSucceed (hash);
        else
            emit lookupArtistDescriptionFailed (hash);
        break;
    }
    case IMetadataLookup::TypeArtistImage: {
        if (succeed)
            emit lookupArtistImageSucceed (hash);
        else
            emit lookupArtistImageFailed (hash);
        break;
    }
    case IMetadataLookup::TypeTrackDescription: {
        if (succeed)
            emit lookupTrackDescriptionSucceed (hash);
        else
            emit lookupTrackDescriptionFailed (hash);
        break;
    }
    case IMetadataLookup::TypeUndefined:
    default: {
        if (!succeed)
            emit metadataLookupFailed (hash);
        break;
    }
    }
}

}//PhoenixPlayer
