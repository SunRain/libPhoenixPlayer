#include "PlayerCore/PlayerCore.h"

#include <QDebug>
#include <QMutex>
#include <QScopedPointer>
#include <QUrl>

#include "Common.h"
#include "Settings.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MetadataLookup/MetadataLookupMgr.h"
#include "SongMetaData.h"
#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "PlayerCore/PlayListMgr.h"
#include "SingletonPointer.h"
#include "Backend/BackendHost.h"

namespace PhoenixPlayer {

using namespace MetadataLookup;
using namespace MusicLibrary;
using namespace PlayBackend;

PlayerCore::PlayerCore(QObject *parent)
    : QObject(parent)
//    ,m_isInit(false)
    ,m_autoSkipForward(true)
{
    m_playBackend = nullptr;
    m_pb = nullptr;
    m_playBackendHost = nullptr;
    m_dao = nullptr;
    m_curTrack = nullptr;

    m_pluginLoader = PluginLoader::instance ();
    m_settings = Settings::instance ();
    m_musicLibraryManager = MusicLibraryManager::instance ();
    MusicLibraryDAOHost *host = m_pluginLoader->curDAOHost ();
    if (host)
        m_dao = host->instance<IMusicLibraryDAO>();

    m_playMode = Common::PlayModeOrder;
    m_playList = new PlayListMgr(this);

    connect (m_playList, &PlayListMgr::currentIndexChanged,
             [&](int index) {
        SongMetaData *data = m_playList->get (index);
        playTrack (data);
    });
    init ();
}

PlayerCore::~PlayerCore()
{
    if (m_playList)
        m_playList->deleteLater ();
    m_playList = nullptr;
}

void PlayerCore::setPluginLoader()
{
    if (!m_pluginLoader) {
        qFatal("[%s] No mPluginLoader", Q_FUNC_INFO);
        return;
    }
//    if (!m_playBackend) {
//        m_playBackend = m_pluginLoader->getCurrentPlayBackend ();
//        if (!PointerValid (EPointer::PPlaybackend))
//            return;
////        qDebug()<<"[PlayerCore] user playbackend "<<mPlayBackend->getPluginName ();
//        m_playBackend->init ();
//        m_playBackend->stop ();
//    }
    m_playBackendHost = m_pluginLoader->curBackendHost ();
    if (!m_playBackendHost || !m_playBackendHost->isValid ()) {
        qCritical()<<Q_FUNC_INFO<<"No playBackendHost or playBackendHost invalid!!";
        return;
    }
    m_pb = m_playBackendHost->instance<IPlayBackend>();
    if (!m_pb) {
        qCritical()<<Q_FUNC_INFO<<"PlayBackend instance fail";
        return;
    }
    m_playBackend = &m_pb;
    (*m_playBackend)->init ();
    (*m_playBackend)->stop ();

//    connect (m_pluginLoader,
//             &PluginLoader::signalPluginChanged,
//             [this](Common::PluginType type) {
//        if (type == Common::PluginPlayBackend) {
//            if (m_playBackend) {
//                m_playBackend->stop ();
//                m_playBackend->deleteLater ();
//            }
//            m_playBackend = m_pluginLoader->getCurrentPlayBackend ();
//            if (!PointerValid (EPointer::PPlaybackend))
//                return;
////            qDebug()<<"change playbackend to"<<mPlayBackend->getPluginName ();
//            m_playBackend->init ();
//            m_playBackend->stop ();
//        }
//    });

//    if (!PointerValid (EPointer::PPlaybackend)) {
//        qDebug()<<"[PlayerCore] No Playbackend found";
//        return;
//    }

    // 播放状态改变信号
//    connect (*m_playBackend, &IPlayBackend::stateChanged,
//             [&](Common::PlayBackendState state) {
//        emit playBackendStateChanged (state);
////        emit playBackendStateChanged ((int)state);
//    });
    connect(*m_playBackend, &IPlayBackend::stateChanged, this, &PlayerCore::playBackendStateChanged);

    //当一首曲目播放结束后
    connect (*m_playBackend,
             &IPlayBackend::finished,
             [&]() {
        emit playTrackFinished ();
        if (m_autoSkipForward)
            doPlayByPlayMode ();
    });

    //播放失败
    connect (*m_playBackend,
             &IPlayBackend::failed,
             [&]() {
        emit playTrackFailed ();
        if (m_autoSkipForward)
            doPlayByPlayMode ();
    });

    //tick
    connect (*m_playBackend,
             &IPlayBackend::tick,
             [&] (quint64 sec) {
        qDebug()<<Q_FUNC_INFO<<"Tick "<<sec;
        m_currentPlayPos = sec;
        emit playTickActual (sec);
        if (m_currentSongLength <= 0)
            return;

        if (m_currentSongLength > 0)
            emit playTickPercent (((qreal)sec/m_currentSongLength) * 100);
    });
}

//void PlayerCore::setMusicLibraryManager()
//{
//    if (m_musicLibraryManager == nullptr) {
//        qDebug()<<"[PlayerCore] no MusicLibraryManager";
//        return;
//    }
//    //播放列表上一首/下一首/随机
//    connect (m_musicLibraryManager,
//             &MusicLibraryManager::playingSongChanged, [this] () {
//        if (!PointerValid (EPointer::PPlaybackend)) {
//            qDebug()<<"[PlayerCore] Can't connect playingSongChanged ";
//            return;
//        }
//        QString playingHash = m_musicLibraryManager->playingSongHash ();
//        m_currentSongLength = getSongLength (playingHash);
//        if (!m_playQueue.contains (playingHash)) {
//            m_playQueue.append (playingHash);
//            qDebug()<<Q_FUNC_INFO<<">>>> append hash to queue "<<playingHash<<" queue size "<<m_playQueue.size ();
//        }

//        emit trackChanged ();

//        PlayBackend::BaseMediaObject obj;
//        obj.setFileName (m_musicLibraryManager->queryOne(playingHash, Common::E_FileName));
//        obj.setFilePath (m_musicLibraryManager->queryOne(playingHash, Common::E_FilePath));
//        obj.setMediaType (Common::MediaTypeLocalFile);
//        qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
//        m_playBackend->changeMedia (&obj, 0, true);
//    });
//}

//void PlayerCore::setMetaLookupManager()
//{
//    if (mMetaLookupManager == nullptr) {
//        mMetaLookupManager = new MetadataLookupMgr(this);//new MetadataLookupManager(this);
//        connect (mMetaLookupManager,
////                 &MetadataLookupManager::queueFinished,
//                 &MetadataLookupMgr::queueFinished,
//                 [this] {
//            qDebug()<<"========>>> queueFinished <<<<========";
//            delete mMetaLookupManager;
//            mMetaLookupManager = nullptr;
//        });

//        connect (mMetaLookupManager,
////                 &MetadataLookupManager::lookupFailed,
//                 &MetadataLookupMgr::lookupFailed,
//                 [this] (const QString &songHash, IMetadataLookup::LookupType type){
//            emitMetadataLookupResult (type, songHash, false);
//        });

//        connect (mMetaLookupManager,
////                 &MetadataLookupManager::lookupSucceed,
//                 &MetadataLookupMgr::lookupSucceed,
//                 [this]
//                 (QString songHash,
//                 QByteArray result,
//                 IMetadataLookup::LookupType type) {
//            SongMetaData meta;
//            meta.setMeta (Common::SongMetaTags::E_Hash, songHash);
//            switch (type) {
//            case IMetadataLookup::TypeAlbumDate:
//                meta.setMeta (Common::E_AlbumYear, result);
//                break;
//            case IMetadataLookup::TypeAlbumDescription:
//                meta.setMeta (Common::E_AlbumDescription, result);
//                break;
//            case IMetadataLookup::TypeAlbumImage:
//                meta.setMeta (Common::E_AlbumImageUrl, result);
//                break;
//            case  IMetadataLookup::TypeArtistDescription:
//                meta.setMeta (Common::E_ArtistDescription, result);
//                break;
//            case IMetadataLookup::TypeArtistImage:
//                meta.setMeta (Common::E_ArtistImageUri, result);
//                break;
//            case IMetadataLookup::TypeLyrics:
//                meta.setMeta (Common::E_Lyrics, result);
//                break;
//            case IMetadataLookup::TypeTrackDescription:
//                meta.setMeta (Common::E_SongDescription, result);
//                break;
//            default:
//                break;
//            }
//            //TODO 也许通过MusicLibraryManager来管理会更好
//            if (PointerValid (EPointer::PPluginLoader)) {
//                MusicLibrary::IPlayListDAO *dao = mPluginLoader->getCurrentPlayListDAO ();
//                if (dao)
//                    dao->updateMetaData (&meta, true);
//            }
//            emitMetadataLookupResult (type, songHash, true);
//        });
//    }
//}

//void PlayerCore::setPlayMode(Common::PlayMode mode)
//{
//    m_playMode = mode;
//    qDebug()<<"Play mode changed to "<<m_playMode;
//    emit playModeChanged (mode);
//    emit playModeChanged (int(mode));
//}

//void PlayerCore::setPlayMode(int mode)
//{
//    setPlayMode (Common::PlayMode(mode));
//}

//Common::PlayMode PlayerCore::getPlayMode()
//{
//    return m_playMode;
//}

//int PlayerCore::getPlayModeInt()
//{
//    return (int)getPlayMode();
//}
Common::PlayMode PlayerCore::playMode() const
{
    return m_playMode;
}

Common::PlayBackendState PlayerCore::playBackendState() const
{
//    return m_playBackendState;
    if (!m_playBackend)
        return Common::PlayBackendStopped;
    return (*m_playBackend)->playBackendState ();
}

//bool PlayerCore::autoSkipForward() const
//{
//    return m_autoSkipForward;
//}

PlayListMgr *PlayerCore::playList() const
{
    return m_playList;
}

SongMetaData *PlayerCore::curTrackMetadata()
{
    return m_curTrack;
}

bool PlayerCore::autoSkipForward() const
{
    return m_autoSkipForward;
}

void PlayerCore::setPlayMode(Common::PlayMode mode)
{
    if (m_playMode != mode) {
        m_playMode = mode;
        emit playModeChanged(mode);
    }
}

void PlayerCore::setAutoSkipForward(bool autoSkipForward)
{
    if (m_autoSkipForward != autoSkipForward) {
        m_autoSkipForward = autoSkipForward;
        emit autoSkipForwardChanged(autoSkipForward);
    }
}

//Common::PlayBackendState PlayerCore::getPlayBackendState()
//{
//    if (!PointerValid((EPointer::PPlaybackend))) {
//        Common::PlayBackendState s = Common::PlayBackendStopped;
//        return s;
//    }
//    return m_playBackend.data()->getPlayBackendState();
//}

//int PlayerCore::getPlayBackendStateInt()
//{
//    return (int)getPlayBackendState();
//}

//void PlayerCore::setAutoSkipForward(bool autoSkipForward)
//{
//    if (m_autoSkipForward == autoSkipForward)
//        return;
//    m_autoSkipForward = autoSkipForward;
//    emit autoSkipForwardChanged ();
//}

//bool PlayerCore::getAutoSkipForward()
//{
//    return m_autoSkipForward;
//}

void PlayerCore::playFromLibrary(const QString &songHah)
{
    if (!m_dao) {
        qCritical("%s : no IMusicLibraryDAO, can't play from library", Q_FUNC_INFO);
        return;
    }
    qDebug()<<Q_FUNC_INFO<<"play for hash "<<songHah;

    SongMetaData *d = m_dao->trackFromHash (songHah);

    qDebug()<<Q_FUNC_INFO<<"find in library "<<d->toString ();

    if (!m_playList->addTrack (d)) {
        d->deleteLater ();
        d = nullptr;
        return;
    }
    m_playList->setCurrentIndex (m_playList->count () -1);
}

void PlayerCore::playFromNetwork(const QUrl &url)
{
    if (!m_playBackend || url.isEmpty () || !url.isValid ())
        return;
    qDebug()<<Q_FUNC_INFO<<url;

//    PlayBackend::BaseMediaObject obj;
//    obj.setFilePath (url.toString ());
//    obj.setMediaType (Common::MediaTypeUrl);
//    (*m_playBackend)->changeMedia (&obj, 0, true);
    SongMetaData d(url);
//    playTrack (&d);
    m_playList->addTrack (&d);
    m_playList->setCurrentIndex (m_playList->count ());
}

void PlayerCore::playTrack(const SongMetaData *data)
{
    if (data) {
        if (m_curTrack)
            m_curTrack->deleteLater ();
        m_curTrack = new SongMetaData(data);
//        emit trackChanged ();
        BaseMediaObject obj;
        obj.setFilePath (data->path ());
        obj.setFileName (data->name ());
        obj.setMediaType ((Common::MediaType)data->mediaType ());
        qDebug()<<Q_FUNC_INFO<<"change file to "<<data->uri ();
        if (*m_playBackend) {
            (*m_playBackend)->changeMedia (&obj, 0, true);
            m_currentPlayPos = 0;
            (*m_playBackend)->play ();
        }
        else
            qCritical("No playBackend found");
        emit trackChanged ();
    } else {
        qWarning()<<Q_FUNC_INFO<<"No SongMetaData found";
    }
}

int PlayerCore::forwardIndex() const
{
    if (m_playList->isEmpty ())
        return -1;

    int index = m_playList->currentIndex () + 1;

    switch (m_playMode) {
    case Common::PlayModeOrder: {
        if (index >= m_playList->count ())
            index = -1;
        break;
    }
    case Common::PlayModeRepeatAll: {
        if (index >= m_playList->count ())
            index = 0;
        break;
    }
    case Common::PlayModeRepeatCurrent:
        index--;
        break;
    case Common::PlayModeShuffle:
        index = - 1;//m_playList->randomIndex ();
        break;
    default:
        break;
    }
    return index;
}

int PlayerCore::backwardIndex() const
{
    if (m_playList->isEmpty ())
        return -1;

    int index = m_playList->currentIndex () -1;

    switch (m_playMode) {
    case Common::PlayModeOrder: {
        if (index < 0)
            index = -1;
        break;
    }
    case Common::PlayModeRepeatAll: {
        if (index < 0)
            index = m_playList->count () -1;
        break;
    }
    case Common::PlayModeRepeatCurrent:
        index++;
        break;
    case Common::PlayModeShuffle:
        index = -1;//m_playList->randomIndex ();
        break;
    default:
        break;
    }
    return index;
}

int PlayerCore::shuffleIndex() const
{
    if (m_playList->isEmpty ())
        return -1;
    QTime time = QTime::currentTime ();
    qsrand(time.second () * 1000 + time.msec ());
    int n = qrand ();
    return n % m_playList->count ();
}

//void PlayerCore::addToQueue(const QString &songHash, bool skipDuplicates)
//{
//    if (!songHash.isEmpty ()) {
//        if (skipDuplicates && m_playQueue.contains (songHash))
//            return;
//        m_playQueue.append (songHash);
//    }
//}

//QStringList PlayerCore::getPlayQueue()
//{
//    return m_playQueue;
//}

//bool PlayerCore::removeQueueItemAt(int index)
//{
//    if (index < 0)
//        return false;
//    if (index > m_playQueue.length () -1)
//        return false;
//    if (m_playQueue.isEmpty ())
//        return false;
//    m_playQueue.removeAt (index);
//    return true;
//}

//void PlayerCore::removeAllQueueItem()
//{
//    m_playQueue.clear ();
//}

//QString PlayerCore::forwardTrackHash(bool jumpToFirst)
//{
//    if (m_musicLibraryManager->getCurrentPlayListHash ().isEmpty () && !m_playQueue.isEmpty ()) {
//        int index = m_playQueue.indexOf (m_musicLibraryManager->playingSongHash ()) +1;
////        qDebug()<<Q_FUNC_INFO<<">>>>>>> next index is "<<index<<" queue size "<<mPlayQueue.size ();
//        if (index >= m_playQueue.size () || m_playQueue.size () == 1) {
//             if (jumpToFirst) index = 0;
//             else return QString();
//        }
//        return m_playQueue.at (index);
//    }
//    if (!jumpToFirst  && (m_musicLibraryManager->playingSongHash () == m_musicLibraryManager->lastSongHash ()))
//        return QString();
//    return m_musicLibraryManager->nextSong (false);
//}

//QString PlayerCore::backwardTrackHash(bool jumpToLast)
//{
//    if (m_musicLibraryManager->getCurrentPlayListHash ().isEmpty () && !m_playQueue.isEmpty ()) {
//        int index = m_playQueue.indexOf (m_musicLibraryManager->playingSongHash ());
////        qDebug()<<Q_FUNC_INFO<<">>>>>>> pre index is "<<index<<" queue size "<<mPlayQueue.size ();
//        if (index == -1) { //no hash found
//            if (jumpToLast)
//                index = 0;
//            else return  QString();
//        } else if (index == 0) { //hash is the first song
//            if (jumpToLast)
//                index = m_playQueue.size () -1; //jump to last song
//            else return QString();
//        } else {
//            index --;
//        }
//        return m_playQueue.at (index);
//    }
//    if (!jumpToLast && (m_musicLibraryManager->lastSongHash () == m_musicLibraryManager->playingSongHash ()))
//        return QString();
//    return m_musicLibraryManager->preSong (false);
//}

//void PlayerCore::lookupLyric(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeLyrics);
//}

//void PlayerCore::lookupAlbumImage(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeAlbumImage);
//}

//void PlayerCore::lookupAlbumDescription(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeAlbumDescription);
//}

//void PlayerCore::lookupAlbumDate(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeAlbumDate);
//}

//void PlayerCore::lookupArtistImage(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeArtistImage);
//}

//void PlayerCore::lookupArtistDescription(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeArtistDescription);
//}

//void PlayerCore::lookupTrackDescription(const QString &songHash)
//{
//    doMetadataLookup (songHash, IMetadataLookup::TypeTrackDescription);
//}

void PlayerCore::togglePlayPause()
{
    if (!m_playBackend)
        return;

    switch ((*m_playBackend)->playBackendState ()) {
    case Common::PlayBackendPlaying:
        (*m_playBackend)->pause ();
        break;
    case Common::PlayBackendPaused:
        (*m_playBackend)->play (m_currentPlayPos);
        break;
    case Common::PlayBackendStopped: {
        if (m_autoSkipForward) {
            qDebug()<<Q_FUNC_INFO<<"playbackend stopped";
//            QString playingHash = m_musicLibraryManager->playingSongHash ();

            SongMetaData *data = m_playList->currentTrack ();
            if (!data)
                break;
            m_currentSongLength = data->trackMeta ()->duration ();//getSongLength (data->trackMeta ()->duration ());
            m_currentPlayPos = 0;

//            //设置播放歌曲的hash,使得MusicLibraryManager发送playingSongChanged信号
//            //此处是为了使得前端qml界面能够在初始化时候刷新
//            m_musicLibraryManager->setPlayingSongHash (playingHash);

//            PlayBackend::BaseMediaObject obj;
//            QStringList list = m_musicLibraryManager
//                    ->querySongMetaElement (Common::E_FileName, playingHash);
//            if (!list.isEmpty ())
//                obj.setFileName (list.first ());
//            list = m_musicLibraryManager
//                    ->querySongMetaElement (Common::E_FilePath, playingHash);
//            if (!list.isEmpty ())
//                obj.setFilePath (list.first ());
//            obj.setMediaType (Common::MediaTypeLocalFile);
//            qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
//            m_playBackend->changeMedia (&obj, 0, true);
            BaseMediaObject obj;
            obj.setFilePath (data->path ());
            obj.setFileName (data->name ());
            obj.setMediaType ((Common::MediaType)data->mediaType ());
            (*m_playBackend)->changeMedia (&obj, 0, true);
        }
        break;
    }
    default:
        break;
    }

}

void PlayerCore::play()
{
    if (m_playBackend)
        (*m_playBackend)->play (m_currentPlayPos);
}

void PlayerCore::stop()
{
    if (m_playBackend)
        //TODO: 是否需要记住最后播放的歌曲(待后续开发情况决定)
        (*m_playBackend)->stop ();
}

void PlayerCore::pause()
{
    if (m_playBackend)
        (*m_playBackend)->pause ();
}

//void PlayerCore::setVolume(int vol)
//{
//    if (PointerValid (EPointer::PPlaybackend))
//        mPlayBackend.data ()->setVolume (vol);
//}

void PlayerCore::setPosition(qreal pos, bool isPercent)
{
    if (!m_playBackend)
        return;

    qDebug()<<"PlayerCore setPosition to "<<pos<<" isPercent "<<isPercent;

    if (isPercent) {
        if (m_currentSongLength <= 0)
            return;
        (*m_playBackend)->setPosition (m_currentSongLength * pos/100);
    } else {
        (*m_playBackend)->setPosition (pos);
    }
}

void PlayerCore::skipForward()
{
    m_playList->setCurrentIndex (this->forwardIndex ());
}

void PlayerCore::skipBackward()
{
    m_playList->setCurrentIndex (this->backwardIndex ());
}

void PlayerCore::skipShuffle()
{
    m_playList->setCurrentIndex (this->shuffleIndex ());
}

//void PlayerCore::skipShuffle()
//{
//    if (m_musicLibraryManager->getCurrentPlayListHash ().isEmpty () && !m_playQueue.isEmpty ()) {
//        QTime time = QTime::currentTime ();
//        qsrand(time.second () * 1000 + time.msec ());
//        int n = qrand ();
//        n = n % m_playQueue.size ();
//        this->playFromLibrary (m_playQueue.at (n));
//    } else {
//        m_musicLibraryManager->randomSong ();
//    }
//}

void PlayerCore::init()
{
    setPluginLoader();
//    setMusicLibraryManager();
//    setMetaLookupManager ();
}

//bool PlayerCore::PointerValid(PlayerCore::EPointer pointer)
//{
//    bool valid = false;
//    switch (pointer) {
//    case EPointer::PPlaybackend:
//        valid = !m_playBackend.isNull ();
//        break;
//    case EPointer::PPluginLoader:
//        valid = (m_pluginLoader != nullptr);
//        break;
//    case EPointer::PMusicLibraryManager:
//        valid = (m_musicLibraryManager != nullptr);
//        break;
////    case EPointer::PPLyricsManager:
////        valid = (mMetaLookupManager != nullptr);
////        break;
//    default:
//        break;
//    }
//    return valid;
//}

int PlayerCore::getSongLength(const QString &hash)
{
//    if (hash.isEmpty ())
//        return 0;

//    if (!PointerValid (EPointer::PMusicLibraryManager))
//        return 0;

//    QStringList list = m_musicLibraryManager
//            ->querySongMetaElement (Common::SongMetaTags::E_TrackLength,
//                                    hash, true);
//    if (!list.isEmpty ()) {
//       return list.first ().toULongLong ();
//    }
//    return 0;
    //TODO dummy
}

//void PlayerCore::doMetadataLookup(const QString &songHash,
//                            IMetadataLookup::LookupType type)
//{
//    setMetaLookupManager ();

//    SongMetaData data;
//    QString hash = songHash;
//    if (hash.isEmpty ()) {
//        qDebug()<<__FUNCTION__<<"hash is empty";
//        hash = mMusicLibraryManager->playingSongHash ();
//    }

//    for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
//         i < (int)Common::SongMetaTags::E_LastFlag;
//         ++i) {
//        QString str = mMusicLibraryManager->queryOne(hash, Common::SongMetaTags(i), true);
//        if (str.isEmpty())
//            data.setMeta (Common::SongMetaTags(i), QVariant());
//        else
//            data.setMeta (Common::SongMetaTags(i), str);

//    }
//    mMetaLookupManager->lookup (&data, type);
//}

//void PlayerCore::emitMetadataLookupResult(IMetadataLookup::LookupType type, const QString &hash, bool succeed)
//{
//    //TODO 添加其他类型的emit
//    switch (type) {
//    case IMetadataLookup::TypeLyrics: {
//        if (succeed)
//            emit lookupLyricSucceed (hash);
//        else
//            emit lookupLyricFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeAlbumDescription: {
//        if (succeed)
//            emit lookupAlbumDescriptionSucceed (hash);
//        else
//            emit lookupAlbumDescriptionFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeAlbumImage: {
//        if (succeed)
//            emit lookupAlbumImageSucceed (hash);
//        else
//            emit lookupAlbumImageFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeAlbumDate: {
//        if (succeed)
//            emit lookupAlbumDateSucceed (hash);
//        else
//            emit lookupAlbumDateFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeArtistDescription: {
//        if (succeed)
//            emit lookupArtistDescriptionSucceed (hash);
//        else
//            emit lookupArtistDescriptionFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeArtistImage: {
//        if (succeed)
//            emit lookupArtistImageSucceed (hash);
//        else
//            emit lookupArtistImageFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeTrackDescription: {
//        if (succeed)
//            emit lookupTrackDescriptionSucceed (hash);
//        else
//            emit lookupTrackDescriptionFailed (hash);
//        break;
//    }
//    case IMetadataLookup::TypeUndefined:
//    default: {
//        if (!succeed)
//            emit metadataLookupFailed (hash);
//        break;
//    }
//    }
//}

void PlayerCore::doPlayByPlayMode()
{
    if (!m_playBackend) {
        qDebug()<<Q_FUNC_INFO<<"Stop due to no playBackend";
        return;
    }
    switch (m_playMode) {
    case Common::PlayModeOrder: { //顺序播放
//        if (this->forwardTrackHash (false).isEmpty ())
//            m_playBackend->stop ();
//        else
//            this->skipForward ();
        if (m_playList->isEmpty ())
            break;
        if (m_playList->currentIndex () >= m_playList->count ()-1)
            (*m_playBackend)->stop ();
        else
            this->skipForward ();
        break;
    }
    case Common::PlayModeRepeatCurrent: { //单曲播放
//        QString playingHash = m_musicLibraryManager->playingSongHash ();
//        PlayBackend::BaseMediaObject obj;

//        QStringList list = m_musicLibraryManager->querySongMetaElement (Common::E_FileName, playingHash);
//        if (!list.isEmpty ())
//            obj.setFileName (list.first ());

//        list = m_musicLibraryManager->querySongMetaElement (Common::E_FilePath, playingHash);
//        if (!list.isEmpty ())
//            obj.setFilePath (list.first ());

//        obj.setMediaType (Common::MediaTypeLocalFile);
//        m_playBackend->changeMedia (&obj, 0, true);

        SongMetaData *data = m_playList->currentTrack ();
        if (data) {
            BaseMediaObject obj;
            obj.setFilePath (data->path ());
            obj.setFileName (data->name ());
            obj.setMediaType ((Common::MediaType)data->mediaType ());
            (*m_playBackend)->changeMedia (&obj, 0, true);
            m_currentPlayPos = 0;
            (*m_playBackend)->play ();
        }
        break;
    }
    case Common::PlayModeRepeatAll:  { //循环播放
        //                mMusicLibraryManager->nextSong ();
        this->skipForward ();
        break;
    }
    case Common::PlayModeShuffle: { //随机播放
        //                mMusicLibraryManager->randomSong ();
        this->skipShuffle ();
        break;
    }
    default:
        break;
    }
}



}//PhoenixPlayer
