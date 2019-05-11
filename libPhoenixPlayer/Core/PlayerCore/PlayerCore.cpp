#include "PlayerCore/PlayerCore.h"

#include <QDebug>
#include <QMutex>
#include <QScopedPointer>
#include <QUrl>

#include "Common.h"
#include "PPSettings.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "MetadataLookup/MetadataLookupMgr.h"
#include "AudioMetaObject.h"
#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "PlayerCore/PlayListObject.h"
#include "PlayerCore/RecentPlayedMgr.h"
#include "PlayerCore/PlayListObjectMgr.h"
#include "SingletonPointer.h"
#include "Backend/BackendHost.h"
#include "MediaResource.h"
#include "PlayerCore/MusicQueue.h"

namespace PhoenixPlayer {

using namespace MetadataLookup;
using namespace MusicLibrary;
using namespace PlayBackend;

PlayerCore::PlayerCore(PPSettings *set, PluginLoader *loader, MusicLibraryManager *mgr, QObject *parent)
    : QObject(parent)
    , m_settings(set)
    , m_pluginLoader(loader)
    , m_musicLibraryManager(mgr)
    , m_resource(nullptr)
{
    m_playBackend = nullptr;
    m_pb = nullptr;
    m_playBackendHost = nullptr;
    m_dao = nullptr;
    m_curTrack = AudioMetaObject();

    m_autoSkipForward = true;

    m_curTrackDuration = 0;
    m_currentPlayPos = 0;

//    m_pluginLoader = phoenixPlayerLib->pluginLoader ();//PluginLoader::instance ();
//    m_settings = phoenixPlayerLib->settings ();//Settings::instance ();
//    m_musicLibraryManager = phoenixPlayerLib->libraryMgr ();//MusicLibraryManager::instance ();
    MusicLibraryDAOHost *host = m_pluginLoader->curDAOHost ();
    if (host)
        m_dao = host->instance<IMusicLibraryDAO>();

    m_playMode = Common::PlayModeOrder;
//    m_playlistObject = new PlayListObject(m_settings, this);
//    m_playlistObject = new PlayListObject(m_settings->playListDir(), this);
//    connect (m_playlistObject, &PlayListObject::currentIndexChanged,
//             [&](int index) {
//        AudioMetaObject o = m_playlistObject->get (index);
//        m_recentList->addTrack (o);
//        playTrack (o);
//    });
//    connect(m_settings, &PPSettings::playListDirChanged,
//             [&](QString arg) {
//        m_playlistObject->setPlayListDir(arg);
//    });

//    m_plstObjMgr = new PlayListObjectMgr(m_settings, this);

    m_recentList = new RecentPlayedMgr(this);
    connect (m_recentList, &RecentPlayedMgr::currentIndexChanged,
             [&](int idx) {
        AudioMetaObject o = m_recentList->get (idx);
        playTrack (o);
    });

    m_playQueue = new MusicQueue(this);
    m_playQueue->setSizeLimit(-1);
    m_playQueue->setSkipDuplicates(false);
    connect(m_playQueue, &MusicQueue::currentIndexChanged,
            [&](int idx) {
        AudioMetaObject o = m_playQueue->get (idx);
        m_recentList->addTrack(o);
        playTrack (o);
    });

//    init ();
}

PlayerCore::~PlayerCore()
{
//    if (m_playlistObject)
//        m_playlistObject->deleteLater ();
//    m_playlistObject = nullptr;
    if (m_recentList)
        m_recentList->deleteLater ();
    m_recentList = nullptr;
    if (m_playQueue)
        m_playQueue->deleteLater();
    m_playQueue = nullptr;
}

void PlayerCore::initiate()
{
    setPluginLoader();
//    setMusicLibraryManager();
//    setMetaLookupManager ();
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

int PlayerCore::playModeInt() const
{
    return (int)m_playMode;
}

Common::PlayBackendState PlayerCore::playBackendState() const
{
//    return m_playBackendState;
    if (!m_playBackend)
        return Common::PlayBackendStopped;
    return (*m_playBackend)->playBackendState ();
}

int PlayerCore::playBackendStateInt() const
{
    return (int)playBackendState ();
}

//bool PlayerCore::autoSkipForward() const
//{
//    return m_autoSkipForward;
//}

//PlayListObject *PlayerCore::playList() const
//{
//    return m_playlistObject;
//}

RecentPlayedMgr *PlayerCore::recentList() const
{
    return m_recentList;
}

MusicQueue *PlayerCore::playQueue() const
{
    return m_playQueue;
}

//QObject *PlayerCore::playListObject() const
//{
//    return qobject_cast<QObject*>(playList ());
//}

AudioMetaObject PlayerCore::curTrackMetadata()
{
    return m_curTrack;
}

QVariantMap PlayerCore::currentTrack() const
{
    return m_curTrack.toMap();
}

bool PlayerCore::autoSkipForward() const
{
    return m_autoSkipForward;
}

void PlayerCore::playAt(int idx)
{
    if (idx < 0 || idx >= m_playQueue->count())
        return;
    m_playQueue->setCurrentIndex(idx);
}

void PlayerCore::playFromLibrary(const QString &songHash)
{
    if (!m_dao) {
        qCritical("%s : no IMusicLibraryDAO, can't play from library", Q_FUNC_INFO);
        return;
    }
    qDebug()<<Q_FUNC_INFO<<"play for hash "<<songHash;

//    AudioMetaObject *d = m_dao->trackFromHash (songHah);
    AudioMetaObject d = m_dao->trackFromHash (songHash);

    qDebug()<<Q_FUNC_INFO<<"find in library "<<d.toMap();

//    m_playList->addTrack (d);
//    m_playList->setCurrentIndex (m_playList->count () -1);
    m_playQueue->addAndFocus(d);
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
    AudioMetaObject d(url);
//    playTrack (&d);
//    m_playList->addTrack (d);
//    m_playList->setCurrentIndex (m_playList->count () -1);
    m_playQueue->addAndFocus(d);
}

void PlayerCore::playTrack(const AudioMetaObject &data)
{
    if (data.isEmpty ()) {
        qWarning()<<Q_FUNC_INFO<<"AudioMetaData is empty";
        return;
    }
    m_curTrack = data;
    m_curTrackDuration = data.trackMeta ().duration ();
//    BaseMediaObject obj;
//    obj.setFilePath (data.path ());
//    obj.setFileName (data.name ());
//    obj.setMediaType ((Common::MediaType)data.mediaType ());
    if (m_resource)
        m_resource->deleteLater ();
    m_resource = nullptr;
    m_resource = MediaResource::create (data.uri ().toString (), 0);
    qDebug()<<Q_FUNC_INFO<<"change file to "<<data.uri ();
    if (*m_playBackend) {
        (*m_playBackend)->changeMedia (m_resource, 0, true);
        m_currentPlayPos = 0;
        (*m_playBackend)->play ();
    }
    else
        qCritical("No playBackend found");
    emit trackChanged (m_curTrack.toMap());
}

int PlayerCore::forwardIndex() const
{
    if (m_playQueue->isEmpty ())
        return -1;

    int index = m_playQueue->currentIndex () + 1;

    switch (m_playMode) {
    case Common::PlayModeOrder: {
        if (index >= m_playQueue->count ())
            index = -1;
        break;
    }
    case Common::PlayModeRepeatAll: {
        if (index >= m_playQueue->count ())
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
    qDebug()<<Q_FUNC_INFO<<" new index is "<<index;

    return index;
}

int PlayerCore::backwardIndex() const
{
    if (m_playQueue->isEmpty ())
        return -1;

    int index = m_playQueue->currentIndex () -1;

    switch (m_playMode) {
    case Common::PlayModeOrder: {
        if (index < 0)
            index = -1;
        break;
    }
    case Common::PlayModeRepeatAll: {
        if (index < 0)
            index = m_playQueue->count () -1;
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
    if (m_playQueue->isEmpty ())
        return -1;
    QTime time = QTime::currentTime ();
    qsrand(time.second () * 1000 + time.msec ());
    int n = qrand ();
    return n % m_playQueue->count ();
}

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

//            AudioMetaObject *data = m_playList->currentTrack ();
            AudioMetaObject data = m_playQueue->currentTrack ();
//            if (!data)
//                break;
            if (data.isEmpty ())
                break;
            m_curTrackDuration = data.trackMeta ().duration ();//data->trackMeta ()->duration ();//getSongLength (data->trackMeta ()->duration ());
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
//            BaseMediaObject obj;
//            obj.setFilePath (/*data->path ()*/data.path ());
//            obj.setFileName (/*data->name ()*/data.name ());
//            obj.setMediaType ((Common::MediaType)/*data->mediaType ()*/data.mediaType ());
//            (*m_playBackend)->changeMedia (&obj, 0, true);
            if (m_resource)
                m_resource->deleteLater ();
            m_resource = MediaResource::create (data.uri ().toString (), this);
            (*m_playBackend)->changeMedia (m_resource, 0, true);
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

void PlayerCore::setPosition(qreal pos, bool isPercent)
{
    if (!m_playBackend)
        return;

    qDebug()<<Q_FUNC_INFO<<"setPosition to "<<pos<<" isPercent "<<isPercent;


    m_currentPlayPos = isPercent
            ? m_curTrackDuration <= 0 ? m_currentPlayPos : m_currentPlayPos * pos/100
            : pos;
//    if (isPercent) {
//        if (m_curTrackDuration <= 0)
//            return;
//        (*m_playBackend)->setPosition (m_curTrackDuration * pos/100);
//    } else {
//        (*m_playBackend)->setPosition (pos);
//    }
    (*m_playBackend)->setPosition (m_currentPlayPos);
}

void PlayerCore::skipForward()
{
    m_playQueue->setCurrentIndex (this->forwardIndex ());
}

void PlayerCore::skipBackward()
{
    m_playQueue->setCurrentIndex (this->backwardIndex ());
}

void PlayerCore::skipShuffle()
{
    m_playQueue->setCurrentIndex (this->shuffleIndex ());
}
void PlayerCore::setPlayMode(Common::PlayMode mode)
{
    if (m_playMode != mode) {
        m_playMode = mode;
        emit playModeChanged(mode);
        emit playModeChanged((int)mode);
    }
}

void PlayerCore::setPlayMode(int mode)
{
//    if ((int)m_playMode != mode) {
//        Common::PlayMode m = Common::PlayMode(mode);
//        m_playMode = m;
//        emit playModeIntChanged(mode);
//    }
    setPlayMode(Common::PlayMode(mode));
}

void PlayerCore::setAutoSkipForward(bool autoSkipForward)
{
    if (m_autoSkipForward != autoSkipForward) {
        m_autoSkipForward = autoSkipForward;
        emit autoSkipForwardChanged(autoSkipForward);
    }
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
    (*m_playBackend)->initialize ();
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
    connect (*m_playBackend, &IPlayBackend::stateChanged,
             [&](Common::PlayBackendState state) {
        emit playBackendStateChanged (state);
//        emit playBackendStateChanged ((int)state);
    });
//    connect(*m_playBackend, &IPlayBackend::stateChanged, this, &PlayerCore::playBackendStateChanged);

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
        qDebug()<<Q_FUNC_INFO<<"Tick "<<sec<<" duration "<<m_curTrackDuration;
        m_currentPlayPos = sec;
        emit playTickActual (sec);
        if (m_curTrackDuration > 0) {
//            qreal v = sec / m_curTrackDuration;
//            int p = v *1000;
            int p = sec/m_curTrackDuration * 100;
            qDebug()<<Q_FUNC_INFO<<" Percent "<<p;
            emit playTickPercent (p);
        }
    });
}

void PlayerCore::doPlayByPlayMode()
{
    if (!m_playBackend) {
        qDebug()<<Q_FUNC_INFO<<"Stop due to no playBackend";
        return;
    }
    qDebug()<<">>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<";
    switch (m_playMode) {
    case Common::PlayModeOrder: { //顺序播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeOrder";
        if (m_playQueue->isEmpty ())
            break;
        if (m_playQueue->currentIndex () >= m_playQueue->count ()-1)
            (*m_playBackend)->stop ();
        else
            this->skipForward ();
        break;
    }
    case Common::PlayModeRepeatCurrent: { //单曲播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeRepeatCurrent";

        AudioMetaObject data = m_playQueue->currentTrack ();
        if (data.isEmpty ())
            break;
//        if (data) {
//            BaseMediaObject obj;
//            obj.setFilePath (/*data->path ()*/data.path ());
//            obj.setFileName (/*data->name ()*/data.name ());
//            obj.setMediaType ((Common::MediaType)/*data->mediaType ()*/data.mediaType ());
        if (m_resource)
            m_resource->deleteLater ();
        m_resource = MediaResource::create (data.uri ().toString (), this);
        (*m_playBackend)->changeMedia (m_resource, 0, true);
        m_currentPlayPos = 0;
        (*m_playBackend)->play ();
//        }
        break;
    }
    case Common::PlayModeRepeatAll:  { //循环播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeRepeatAll";
        this->skipForward ();
        break;
    }
    case Common::PlayModeShuffle: { //随机播放
        qDebug()<<Q_FUNC_INFO<<" PlayModeShuffle";
        this->skipShuffle ();
        break;
    }
    default:
        break;
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

//void PlayerCore::playFromLibrary(const QString &songHah)
//{
//    if (!m_dao) {
//        qCritical("%s : no IMusicLibraryDAO, can't play from library", Q_FUNC_INFO);
//        return;
//    }
//    qDebug()<<Q_FUNC_INFO<<"play for hash "<<songHah;

////    AudioMetaObject *d = m_dao->trackFromHash (songHah);
//    AudioMetaObject d = m_dao->trackFromHash (songHah);

//    qDebug()<<Q_FUNC_INFO<<"find in library "<<d.toMap();

////    m_playList->addTrack (d);
////    m_playList->setCurrentIndex (m_playList->count () -1);
//    m_playQueue->addAndFocus(d);
//}

//void PlayerCore::playFromNetwork(const QUrl &url)
//{
//    if (!m_playBackend || url.isEmpty () || !url.isValid ())
//        return;
//    qDebug()<<Q_FUNC_INFO<<url;

////    PlayBackend::BaseMediaObject obj;
////    obj.setFilePath (url.toString ());
////    obj.setMediaType (Common::MediaTypeUrl);
////    (*m_playBackend)->changeMedia (&obj, 0, true);
//    AudioMetaObject d(url);
////    playTrack (&d);
////    m_playList->addTrack (d);
////    m_playList->setCurrentIndex (m_playList->count () -1);
//    m_playQueue->addAndFocus(d);
//}

//void PlayerCore::playTrack(const AudioMetaObject &data)
//{
//    if (data.isEmpty ()) {
//        qWarning()<<Q_FUNC_INFO<<"AudioMetaData is empty";
//        return;
//    }
//    m_curTrack = data;
//    m_curTrackDuration = data.trackMeta ().duration ();
////    BaseMediaObject obj;
////    obj.setFilePath (data.path ());
////    obj.setFileName (data.name ());
////    obj.setMediaType ((Common::MediaType)data.mediaType ());
//    if (m_resource)
//        m_resource->deleteLater ();
//    m_resource = nullptr;
//    m_resource = MediaResource::create (data.uri ().toString (), 0);
//    qDebug()<<Q_FUNC_INFO<<"change file to "<<data.uri ();
//    if (*m_playBackend) {
//        (*m_playBackend)->changeMedia (m_resource, 0, true);
//        m_currentPlayPos = 0;
//        (*m_playBackend)->play ();
//    }
//    else
//        qCritical("No playBackend found");
//    emit trackChanged (m_curTrack.toMap());
//}

//int PlayerCore::forwardIndex() const
//{
//    if (m_playQueue->isEmpty ())
//        return -1;

//    int index = m_playQueue->currentIndex () + 1;

//    switch (m_playMode) {
//    case Common::PlayModeOrder: {
//        if (index >= m_playQueue->count ())
//            index = -1;
//        break;
//    }
//    case Common::PlayModeRepeatAll: {
//        if (index >= m_playQueue->count ())
//            index = 0;
//        break;
//    }
//    case Common::PlayModeRepeatCurrent:
//        index--;
//        break;
//    case Common::PlayModeShuffle:
//        index = - 1;//m_playList->randomIndex ();
//        break;
//    default:
//        break;
//    }
//    qDebug()<<Q_FUNC_INFO<<" new index is "<<index;

//    return index;
//}

//int PlayerCore::backwardIndex() const
//{
//    if (m_playQueue->isEmpty ())
//        return -1;

//    int index = m_playQueue->currentIndex () -1;

//    switch (m_playMode) {
//    case Common::PlayModeOrder: {
//        if (index < 0)
//            index = -1;
//        break;
//    }
//    case Common::PlayModeRepeatAll: {
//        if (index < 0)
//            index = m_playQueue->count () -1;
//        break;
//    }
//    case Common::PlayModeRepeatCurrent:
//        index++;
//        break;
//    case Common::PlayModeShuffle:
//        index = -1;//m_playList->randomIndex ();
//        break;
//    default:
//        break;
//    }
//    return index;
//}

//int PlayerCore::shuffleIndex() const
//{
//    if (m_playQueue->isEmpty ())
//        return -1;
//    QTime time = QTime::currentTime ();
//    qsrand(time.second () * 1000 + time.msec ());
//    int n = qrand ();
//    return n % m_playQueue->count ();
//}



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

//void PlayerCore::togglePlayPause()
//{
//    if (!m_playBackend)
//        return;

//    switch ((*m_playBackend)->playBackendState ()) {
//    case Common::PlayBackendPlaying:
//        (*m_playBackend)->pause ();
//        break;
//    case Common::PlayBackendPaused:
//        (*m_playBackend)->play (m_currentPlayPos);
//        break;
//    case Common::PlayBackendStopped: {
//        if (m_autoSkipForward) {
//            qDebug()<<Q_FUNC_INFO<<"playbackend stopped";
////            QString playingHash = m_musicLibraryManager->playingSongHash ();

////            AudioMetaObject *data = m_playList->currentTrack ();
//            AudioMetaObject data = m_playQueue->currentTrack ();
////            if (!data)
////                break;
//            if (data.isEmpty ())
//                break;
//            m_curTrackDuration = data.trackMeta ().duration ();//data->trackMeta ()->duration ();//getSongLength (data->trackMeta ()->duration ());
//            m_currentPlayPos = 0;

////            //设置播放歌曲的hash,使得MusicLibraryManager发送playingSongChanged信号
////            //此处是为了使得前端qml界面能够在初始化时候刷新
////            m_musicLibraryManager->setPlayingSongHash (playingHash);

////            PlayBackend::BaseMediaObject obj;
////            QStringList list = m_musicLibraryManager
////                    ->querySongMetaElement (Common::E_FileName, playingHash);
////            if (!list.isEmpty ())
////                obj.setFileName (list.first ());
////            list = m_musicLibraryManager
////                    ->querySongMetaElement (Common::E_FilePath, playingHash);
////            if (!list.isEmpty ())
////                obj.setFilePath (list.first ());
////            obj.setMediaType (Common::MediaTypeLocalFile);
////            qDebug()<<"Change song to " << obj.filePath ()<<"  "<<obj.fileName ();
////            m_playBackend->changeMedia (&obj, 0, true);
////            BaseMediaObject obj;
////            obj.setFilePath (/*data->path ()*/data.path ());
////            obj.setFileName (/*data->name ()*/data.name ());
////            obj.setMediaType ((Common::MediaType)/*data->mediaType ()*/data.mediaType ());
////            (*m_playBackend)->changeMedia (&obj, 0, true);
//            if (m_resource)
//                m_resource->deleteLater ();
//            m_resource = MediaResource::create (data.uri ().toString (), this);
//            (*m_playBackend)->changeMedia (m_resource, 0, true);
//        }
//        break;
//    }
//    default:
//        break;
//    }
//}

//void PlayerCore::play()
//{
//    if (m_playBackend)
//        (*m_playBackend)->play (m_currentPlayPos);
//}

//void PlayerCore::stop()
//{
//    if (m_playBackend)
//        //TODO: 是否需要记住最后播放的歌曲(待后续开发情况决定)
//        (*m_playBackend)->stop ();
//}

//void PlayerCore::pause()
//{
//    if (m_playBackend)
//        (*m_playBackend)->pause ();
//}

//void PlayerCore::setVolume(int vol)
//{
//    if (PointerValid (EPointer::PPlaybackend))
//        mPlayBackend.data ()->setVolume (vol);
//}

//void PlayerCore::setPosition(qreal pos, bool isPercent)
//{
//    if (!m_playBackend)
//        return;

//    qDebug()<<Q_FUNC_INFO<<"setPosition to "<<pos<<" isPercent "<<isPercent;


//    m_currentPlayPos = isPercent
//            ? m_curTrackDuration <= 0 ? m_currentPlayPos : m_currentPlayPos * pos/100
//            : pos;
////    if (isPercent) {
////        if (m_curTrackDuration <= 0)
////            return;
////        (*m_playBackend)->setPosition (m_curTrackDuration * pos/100);
////    } else {
////        (*m_playBackend)->setPosition (pos);
////    }
//    (*m_playBackend)->setPosition (m_currentPlayPos);
//}


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

//void PlayerCore::initiate()
//{
//    setPluginLoader();
////    setMusicLibraryManager();
////    setMetaLookupManager ();
//}

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

//int PlayerCore::getSongLength(const QString &hash)
//{
////    if (hash.isEmpty ())
////        return 0;

////    if (!PointerValid (EPointer::PMusicLibraryManager))
////        return 0;

////    QStringList list = m_musicLibraryManager
////            ->querySongMetaElement (Common::SongMetaTags::E_TrackLength,
////                                    hash, true);
////    if (!list.isEmpty ()) {
////       return list.first ().toULongLong ();
////    }
////    return 0;
////    TODO dummy
//}

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





}//PhoenixPlayer
