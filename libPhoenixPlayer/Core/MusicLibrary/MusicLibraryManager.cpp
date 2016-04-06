
#include "MusicLibrary/MusicLibraryManager.h"

#include <QThread>
#include <QDebug>
#include <QPointer>
#include <QMutex>

#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "Settings.h"
#include "Utility.h"
#include "AudioMetaObject.h"
#include "Common.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : BaseObject(parent)
{
    m_settings = Settings::instance ();
    m_pluginLoader = PluginLoader::instance ();
    m_dao = nullptr;
    m_daoHost = m_pluginLoader->curDAOHost ();
    if (m_daoHost)
        m_dao = m_daoHost->instance<IMusicLibraryDAO>();

    if (m_dao) {
        connect (m_dao, &IMusicLibraryDAO::metaDataInserted,
                 [&]() {
//            if (!trackInList (data)) {
//                SongMetaData *d = new SongMetaData(data);
//                m_dao->fillAttribute (&d);
//                m_trackList.append (d);
//            }
            QStringList daoHashList = m_dao->trackHashList ();
            QStringList trackList;
            foreach (const AudioMetaObject &d, m_trackList) {
                trackList.append (d.hash ());
            }
            foreach (QString s, daoHashList) {
                if (!trackList.contains (s)) {
//                    AudioMetaObject *d = m_dao->trackFromHash (s);
//                    if (d) {
//                        AudioMetaObject *dd = new AudioMetaObject(d);
//                        m_trackList.append (dd);
//                        d->deleteLater ();
//                    }
//                    d = nullptr;
                    AudioMetaObject o = m_dao->trackFromHash (s);
                    m_trackList.append (o);
                }
            }
        });
        connect (m_dao, &IMusicLibraryDAO::metaDataDeleted,
                 [&]() {
//            if (trackInList (data)) {
//                int i;
//                for(i=0; i<m_trackList.size (); ++i) {
//                    if (m_trackList.at (i)->equals (*data))
//                        break;
//                }
//                m_trackList.removeAt (i);
//            }
            QStringList daoList = m_dao->trackHashList ();
            QStringList trackList;
            foreach (AudioMetaObject d, m_trackList) {
                trackList.append (d.hash ());
            }
            int pos = -1;
            for (int i=0; i<trackList.size (); ++i) {
                if (!daoList.contains (trackList.value (i))) {
                    pos = i;
                    break;
                }
            }
            if (pos > 0)
                m_trackList.removeAt (pos);
        });
    }

    initList ();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";

    if (m_dao)
        m_dao = nullptr;
    if (m_daoHost->isLoaded ()) {
        if (!m_daoHost->unLoad ())
            m_daoHost->forceUnload ();
    }
    if (!m_trackList.isEmpty ()) {
//        qDeleteAll(m_trackList);
        m_trackList.clear ();
    }

    qDebug()<<">>>>>>>> after "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";
}

QList<AudioMetaObject> MusicLibraryManager::allTracks()
{
//    return m_trackList;
//    QList<QObject*> list;
//    foreach (AudioMetaObject *d, m_trackList) {
//        QObject *obj = qobject_cast<QObject *>(d);
//        if (obj)
//            list.append (obj);
//    }
//    return list;
    return m_trackList;
}

bool MusicLibraryManager::empty() const
{
    return m_trackList.isEmpty ();
}

QList<AudioMetaObject> MusicLibraryManager::artistTracks(const QString &artistName, int limitNum)
{
    if (m_trackList.isEmpty ())
        return QList<AudioMetaObject>();
    QList<AudioMetaObject> list;
    int i = 0;
    foreach (AudioMetaObject d, m_trackList) {
        if (d.artistMeta ().name () == artistName) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    qDebug()<<Q_FUNC_INFO<<"list size is "<<list.size ();
    return list;
}

QList<AudioMetaObject> MusicLibraryManager::albumTracks(const QString &albumName, int limitNum)
{
    if (m_trackList.isEmpty ())
        return QList<AudioMetaObject>();
    QList<AudioMetaObject> list;
    int i = 0;
    foreach (AudioMetaObject d, m_trackList) {
        if (d.albumMeta ().name () == albumName) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

QList<AudioMetaObject> MusicLibraryManager::genreTracks(const QString &genreName, int limitNum)
{
    if (m_trackList.isEmpty ())
        return QList<AudioMetaObject>();
    QList<AudioMetaObject> list;
    int i = 0;
    foreach (AudioMetaObject d, m_trackList) {
        if (d.trackMeta ().genre () == genreName) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

QList<AudioMetaObject> MusicLibraryManager::mediaTypeTracks(const QString &mediaType, int limitNum)
{
    if (m_trackList.isEmpty ())
        return QList<AudioMetaObject>();
    QList<AudioMetaObject> list;
    int i = 0;
    foreach (AudioMetaObject d, m_trackList) {
        if (d.mediaType () == mediaType.toInt ()) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

QList<AudioMetaObject> MusicLibraryManager::userRatingTracks(const QString &rating, int limitNum)
{
    if (m_trackList.isEmpty ())
        return QList<AudioMetaObject>();
    QList<AudioMetaObject> list;
    int i = 0;
    foreach (AudioMetaObject d, m_trackList) {
        if (d.trackMeta ().userRating () == rating) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

QList<AudioMetaObject> MusicLibraryManager::folderTracks(const QString &folder, int limitNum)
{
    if (m_trackList.isEmpty ())
        return QList<AudioMetaObject>();
    QList<AudioMetaObject> list;
    int i = 0;
    foreach (AudioMetaObject d, m_trackList) {
        if (d.path () == folder) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

void MusicLibraryManager::initList()
{
    if (!m_trackList.isEmpty ()) {
//        qDeleteAll(m_trackList);
        m_trackList.clear ();
    }
    if (!m_dao)
        return;
    QStringList list = m_dao->trackHashList ();
    foreach (QString s, list) {
//        AudioMetaObject *d = m_dao->trackFromHash (s);
//        if (d) {
//            AudioMetaObject *dd = new AudioMetaObject(d);
//            m_dao->fillAttribute (&dd);
//            m_trackList.append (dd);
//        }
        AudioMetaObject d = m_dao->trackFromHash (s);
        if (!d.isEmpty ())
            m_trackList.append (d);
    }
}

//inline bool MusicLibraryManager::trackInList(AudioMetaObject **data)
//{
//    if (m_trackList.isEmpty ())
//        return false;
//    foreach (AudioMetaObject *d, m_trackList) {
//        if (d->equals (*data))
//            return true;
//    }
//    return false;
//}

//TrackGroupObject::TrackGroupObject(const QString &name, const QUrl &imgUri, QObject *parent)
//    : BaseObject(parent)
//{
//    m_name = name;
//    m_imgUri = imgUri;
//}

//QUrl TrackGroupObject::imgUri() const
//{
//    return m_imgUri;
//}

//QString TrackGroupObject::name() const
//{
//    return m_name;
//}

//bool MusicLibraryManager::changePlayList(const QString &playListHash)
//{
//    if (m_currentPlayListHash == playListHash) {
//        qDebug()<<"Same playList, not changed";
//        return false;
//    }
//    m_currentPlayListHash = playListHash;
//    m_currentSongHash = QString();
//    emit playListChanged ();
//    return true;
//}

//bool MusicLibraryManager::createPlayList(const QString &playListName)
//{
//    if (m_playListDAO->insertPlayList (playListName)) {
//        emit playListCreated ();
//        return true;
//    }
//    return false;
//}

//bool MusicLibraryManager::deletePlayList(const QString &playListHash)
//{
//    if (m_playListDAO->deletePlayList (playListHash)) {
//        emit playListDeleted ();
//        return true;
//    }
//    return false;
//}

//QString MusicLibraryManager::getCurrentPlayListHash()
//{
//    return m_currentPlayListHash;
//}

//QString MusicLibraryManager::playingSongHash()
//{
//    if (m_currentSongHash.isEmpty ()) {
//        qDebug()<<"try playingSongHash from settings";
//        m_currentSongHash = m_settings->lastPlayedSong ();
//        if (m_currentSongHash.isEmpty ()
//                && !m_playListDAO->getSongHashList (m_currentPlayListHash).isEmpty ()) {
//            qDebug()<<"try playingSongHash from first from library";
//            m_currentSongHash = m_playListDAO->getSongHashList (m_currentPlayListHash).first ();
//        } else {
//            qDebug()<<Q_FUNC_INFO<<"get some error";
//        }
//    }
//    qDebug()<<"playingSongHash is "<<m_currentSongHash;
//    return m_currentSongHash;
//}

//void MusicLibraryManager::setPlayingSongHash(const QString &newHash)
//{
//    if (newHash.isEmpty ())
//        return;
//    if (m_currentSongHash == newHash)
//        return;
//    qDebug()<<Q_FUNC_INFO<<"change current song hash from "<<m_currentSongHash<<" to "<<newHash;
//    m_currentSongHash = newHash;
//    emit playingSongChanged ();
//}

//QString MusicLibraryManager::firstSongHash()
//{
//    m_currentSongHash = m_playListDAO->getSongHashList (m_currentPlayListHash).first ();
//    return m_currentSongHash;
//}

//QString MusicLibraryManager::lastSongHash()
//{
//    m_currentSongHash = m_playListDAO->getSongHashList (m_currentPlayListHash).last ();
//    return m_currentSongHash;
//}

//QString MusicLibraryManager::nextSong(bool jumpToNextSong)
//{
//    QStringList list = m_playListDAO->getSongHashList (m_currentPlayListHash);
//    if (!list.isEmpty ()) {
//        int index = list.indexOf (m_currentSongHash) +1;
//        if (index >= list.size ())
//            index = 0;
//        if (jumpToNextSong) {
//            m_currentSongHash = list.at (index);
//            emit playingSongChanged ();
//        }
//        return list.at (index);
//    }
//    return QString();
//}

//QString MusicLibraryManager::preSong(bool jumpToPreSong)
//{
//    QStringList list = m_playListDAO->getSongHashList (m_currentPlayListHash);
//    if (!list.isEmpty ()) {
//        int index = list.indexOf (m_currentSongHash);
//        if (index == -1) { //no hash found
//            index = 0;
//        } else if (index == 0) { //hash is the first song
//            index = list.size () -1; //jump to last song
//        } else {
//            index --;
//        }
//        if (jumpToPreSong) {
//            m_currentSongHash = list.at (index);
//            emit playingSongChanged ();
//        }
//        return list.at (index);
//    }
//    return QString();
//}

//QString MusicLibraryManager::randomSong(bool jumpToRandomSong)
//{
//    QStringList list = m_playListDAO->getSongHashList (m_currentPlayListHash);
//    if (!list.isEmpty ()) {
//        QTime time = QTime::currentTime ();
//        qsrand(time.second () * 1000 + time.msec ());
//        int n = qrand ();
//        n = n % list.size ();
//        if (jumpToRandomSong) {
//            m_currentSongHash = list.at (n);
//            emit playingSongChanged ();
//        }
//        return list.at (n);
//    }
//    return QString();
//}

//QString MusicLibraryManager::queryOneByIndex(const QString &hash, int tag, bool skipDuplicates)
//{
//    return queryOne(hash, Common::SongMetaTags(tag), skipDuplicates);
//}

//QStringList MusicLibraryManager::queryMusicLibrary(Common::SongMetaTags targetColumn,
//                                                   Common::SongMetaTags regColumn,
//                                                   const QString &regValue,
//                                                   bool skipDuplicates)
//{
//    return m_playListDAO->queryMusicLibrary(targetColumn, regColumn, regValue, skipDuplicates);
//}

//QString MusicLibraryManager::querySongImageUri(const QString &hash)
//{
//    QString uri = queryOne(hash, Common::E_CoverArtMiddle);
//    if (uri.isEmpty())
//        uri = queryOne(hash, Common::E_CoverArtLarge);
//    if (uri.isEmpty())
//        uri = queryOne(hash, Common::E_CoverArtSmall);
//    if (uri.isEmpty())
//        uri = queryOne(hash, Common::E_AlbumImageUrl);
//    if (uri.isEmpty())
//        uri = queryOne(hash, Common::E_ArtistImageUri);
//    return uri;
//}

//QString MusicLibraryManager::querySongTitle(const QString &hash)
//{
//    QString str = queryOne(hash, Common::E_TrackTitle);
//    if (str.isEmpty()) {
//        str = queryOne(hash, Common::E_FileName);
//        if (!str.isEmpty())
//            str = str.mid(0, str.lastIndexOf("."));
//    }
//    return str;
//}

//QStringList MusicLibraryManager::querySongMetaElement(Common::SongMetaTags targetColumn,
//                                                      const QString &hash,
//                                                      bool skipDuplicates)
//{
//    QStringList list;
//    if (hash.isEmpty ()) {
//        list = m_playListDAO
//                ->queryMusicLibrary (targetColumn,
//                                     Common::SongMetaTags::E_FirstFlag, //UnUsed
//                                     QString(),
//                                     skipDuplicates);
//    } else {
//        list = m_playListDAO
//                ->queryMusicLibrary (targetColumn,
//                                     Common::SongMetaTags::E_Hash,
//                                     hash,
//                                     skipDuplicates);
//    }
//    return list;
//}

//QStringList MusicLibraryManager::querySongMetaElementByIndex(
//        int columnIndex, const QString &hash, bool skipDuplicates)
//{
//    return querySongMetaElement (Common::SongMetaTags(columnIndex), hash, skipDuplicates);
//}

//QStringList MusicLibraryManager::queryPlayListElement(
//        Common::PlayListElement targetColumn, const QString &hash)
//{
//    QStringList list;
//    if (hash.isEmpty ()) {
//        list = m_playListDAO->queryPlayList (targetColumn, Common::PlayListFirstFlag, QString());
//    } else {
//        list = m_playListDAO->queryPlayList (targetColumn, Common::PlayListHash, hash);
//    }

//    qDebug()<<" query result "<< list;
//    return list;
//}

//QStringList MusicLibraryManager::queryPlayListElementByIndex(int index, const QString &hash)
//{
//    return queryPlayListElement (Common::PlayListElement(index), hash);
//}

//bool MusicLibraryManager::insertToPlayList(const QString &playListHash,
//                                           const QString &newSongHash)
//{
//    if (playListHash.isEmpty () || newSongHash.isEmpty ()) {
//        qDebug()<<"playListHash or newSongHash is empty";
//        return false;
//    }
//    bool ret = m_playListDAO->updatePlayList (Common::PlayListSongHashes,
//                                                    playListHash,
//                                                    newSongHash,
//                                                    true);
//    if (ret)
//        emit playListTrackChanged ();
//    return ret;
//}

//bool MusicLibraryManager::deleteFromPlayList(
//        const QString &playListHash, const QString &songHash, bool deleteFromStorage)
//{
//    Q_UNUSED(deleteFromStorage)

//    if (playListHash.isEmpty () || songHash.isEmpty ()) {
//        qDebug()<<"playListHash or newSongHash is empty";
//        return false;
//    }
//    //TODO: 需要添加从本地删除的功能
//    bool ret = m_playListDAO->updatePlayList (Common::PlayListSongHashes,
//                                                    playListHash,
//                                                    songHash,
//                                                    false);
//    if (ret)
//        emit playListTrackChanged ();
//    return ret;
//}

//bool MusicLibraryManager::init()
//{
//    qDebug()<<QString(">>>>>>>>>> %1 <<<<<<<<<").arg (Q_FUNC_INFO);

//    m_currentSongHash = m_settings->lastPlayedSong ();
//    m_currentPlayListHash = m_settings->getPlayListHash ();

//    if (!m_playListDAO)
//        m_playListDAO = m_pluginLoader->getCurrentLibraryDAO ();
//    if (m_playListDAO) {
//        if (!m_playListDAO->initDataBase ()) {
//            qDebug()<<"initDataBase error";
//        }
//    } else {
//        qDebug()<<"Can't find mPlayListDAO";
//    }
//    m_isInit = true;

//    return true;
//}

//QString MusicLibraryManager::queryOne(const QString &hash, Common::SongMetaTags tag, bool skipDuplicates)
//{
//    if (hash.isEmpty())
//        return QString();
//    QStringList list = this->querySongMetaElement(tag, hash, skipDuplicates);
//    if (list.isEmpty())
//        return QString();
//    return list.first();
//}

} //MusicLibrary
} //PhoenixPlayer

