
#include "MusicLibrary/MusicLibraryManager.h"

#include <QThread>
#include <QDebug>
#include <QPointer>
#include <QMutex>
#include <QDateTime>

#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/MusicLibraryDAOHost.h"
#include "MusicLibrary/LocalMusicScanner.h"
#include "PPSettings.h"
#include "PPUtility.h"
#include "AudioMetaObject.h"
#include "PPCommon.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

MusicLibraryManager::MusicLibraryManager(PPSettings *set, PluginLoader *loader, QObject *parent)
    : BaseObject(parent),
      m_settings(set),
      m_pluginLoader(loader),
      m_localMusicScanner(new LocalMusicScanner(m_settings, m_pluginLoader))
{
//    m_settings = phoenixPlayerLib->settings ();//Settings::instance ();
//    m_pluginLoader = phoenixPlayerLib->pluginLoader ();//PluginLoader::instance ();
    m_dao = nullptr;
    m_daoHost = m_pluginLoader->curDAOHost ();
    if (m_daoHost)
        m_dao = m_daoHost->instance<IMusicLibraryDAO>();

    if (m_dao) {
        connect (m_dao, &IMusicLibraryDAO::metaDataInserted,
                 this, [&](const QString &hash) {
            QStringList daoHashList = m_dao->trackHashList ();
            QStringList trackList;
            foreach (const AudioMetaObject &d, m_trackList) {
                trackList.append (d.hash ());
            }
            foreach (const QString &s, daoHashList) {
                if (!trackList.contains (s)) {
                    AudioMetaObject o = m_dao->trackFromHash (s);
                    m_trackList.append (o);
                    this->insertToAlbumGroupMap(o);
                    this->insertToGenreGroupMap(o);
                    this->insertToArtistGroupMap(o);
                }
            }
        });
        connect (m_dao, &IMusicLibraryDAO::metaDataDeleted,
                 this, [&](const QString &hash) {
//            QStringList daoList = m_dao->trackHashList ();
//            QStringList trackList;
//            foreach (const AudioMetaObject &d, m_trackList) {
//                trackList.append (d.hash ());
//            }
//            int pos = -1;
//            for (int i=0; i<trackList.size (); ++i) {
//                if (!daoList.contains (trackList.value (i))) {
//                    pos = i;
//                    break;
//                }
//            }
//            if (pos > 0) {
//                m_trackList.removeAt (pos);
//            }
            static auto loop = [](const AudioMetaList &list, const QString &hash) -> int {
                int pos = -1;
                for (int i=0; i<list.size(); ++i) {
                    if (list.value(i).hash() == hash) {
                        pos = i;
                        break;
                    }
                }
                return pos;
            };
            int pos = loop(m_trackList, hash);
            if (pos > 0) {
//                m_trackList.removeAt(pos);
                const AudioMetaObject obj = m_trackList.at(pos);
                if (m_albumGroupMap.contains(obj.albumMeta().name())) {
                    AudioMetaGroupObject o = m_albumGroupMap.value(obj.albumMeta().name());
                    AudioMetaList list = o.list();
                    int pp = loop(list, hash);
                    if (pp > 0) {
                        list.removeAt(pp);
                        o.setList(list);
                        m_albumGroupMap.insert(obj.albumMeta().name(), o);
                    }
                }
                //copy
                if (m_artistGroupMap.contains(obj.artistMeta().name())) {
                    AudioMetaGroupObject o = m_artistGroupMap.value(obj.artistMeta().name());
                    AudioMetaList list = o.list();
                    int pp = loop(list, hash);
                    if (pp > 0) {
                        list.removeAt(pp);
                        o.setList(list);
                        m_artistGroupMap.insert(obj.artistMeta().name(), o);
                    }
                }
                //copy
                if (m_genreGroupMap.contains(obj.trackMeta().genre())) {
                    AudioMetaGroupObject o = m_genreGroupMap.value(obj.trackMeta().genre());
                    AudioMetaList list = o.list();
                    int pp = loop(list, hash);
                    if (pp > 0) {
                        list.removeAt(pp);
                        o.setList(list);
                        m_genreGroupMap.insert(obj.trackMeta().genre(), o);
                    }
                }
                m_trackList.removeAt(pos);
            }
        });
    }

    initList();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";
    saveToDB();
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

LocalMusicScanner *MusicLibraryManager::localMusicScanner() const
{
    return m_localMusicScanner;
}

AudioMetaList MusicLibraryManager::allTracks()
{
    return m_trackList;
}

AudioMetaObject MusicLibraryManager::trackFromHash(const QString &hash) const
{
    if (!m_dao) {
        return AudioMetaObject();
    }
    return m_dao->trackFromHash(hash);
}

bool MusicLibraryManager::empty() const
{
    return m_trackList.isEmpty ();
}

AudioMetaList MusicLibraryManager::artistTracks(const QString &artistName, int limitNum)
{
    if (m_trackList.isEmpty ())
        return AudioMetaList();
    AudioMetaList list;
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

AudioMetaList MusicLibraryManager::albumTracks(const QString &albumName, int limitNum)
{
    if (m_trackList.isEmpty ())
        return AudioMetaList();
    AudioMetaList list;
    int i = 0;
    foreach (const auto &d, m_trackList) {
        if (d.albumMeta ().name () == albumName) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

AudioMetaList MusicLibraryManager::genreTracks(const QString &genreName, int limitNum)
{
    if (m_trackList.isEmpty ())
        return AudioMetaList();
    AudioMetaList list;
    int i = 0;
    foreach (const auto &d, m_trackList) {
        if (d.trackMeta ().genre () == genreName) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

AudioMetaList MusicLibraryManager::mediaTypeTracks(const QString &mediaType, int limitNum)
{
    if (m_trackList.isEmpty ())
        return AudioMetaList();
    AudioMetaList list;
    int i = 0;
    foreach (const auto &d, m_trackList) {
        if (d.mediaType () == mediaType.toInt ()) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

AudioMetaList MusicLibraryManager::userRatingTracks(const QString &rating, int limitNum)
{
    if (m_trackList.isEmpty ())
        return AudioMetaList();
    AudioMetaList list;
    int i = 0;
    foreach (const auto &d, m_trackList) {
        if (d.trackMeta ().userRating () == rating) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

AudioMetaList MusicLibraryManager::folderTracks(const QString &folder, int limitNum)
{
    if (m_trackList.isEmpty ())
        return AudioMetaList();
    AudioMetaList list;
    int i = 0;
    foreach (const auto &d, m_trackList) {
        if (d.path () == folder) {
            list.append (d);
        }
        i++;
        if (limitNum > 0 && i >= limitNum)
            break;
    }
    return list;
}

AudioMetaGroupList MusicLibraryManager::artistList() const
{
//    if (m_trackList.isEmpty ())
//        return AudioMetaGroupList();
//    AudioMetaGroupList list;
//    QStringList nameList;
//    foreach (const AudioMetaObject &o, m_trackList) {
//        if (nameList.contains (o.artistMeta ().name ()))
//            continue;
//        nameList.append (o.artistMeta ().name ());
//    }
//    foreach (const QString &name, nameList) {
//        AudioMetaGroupObject g;
//        g.setName (name);
//        QUrl uri;
//        AudioMetaList ll;
//        foreach (const AudioMetaObject &o, m_trackList) {
//            if (o.artistMeta ().name () == name) {
//                ll.append (o);
//                if (!uri.isValid ()) {
//                    uri = o.artistMeta ().imgUri ();
//                }
//            }
//        }
//        //fallback
//        if (!uri.isValid () && !ll.isEmpty ()) {
//            foreach (const AudioMetaObject &o, ll) {
//                uri = o.queryImgUri ();
//                if (uri.isValid ()) {
//                    break;
//                }
//            }
//        }
//        g.setImageUri (uri);
//        g.setList (ll);
//        list.append (g);
//    }
//    return list;
    return m_artistGroupMap.values();
}

AudioMetaGroupList MusicLibraryManager::albumList() const
{
//    if (m_trackList.isEmpty ())
//        return AudioMetaGroupList();
//    AudioMetaGroupList list;
//    QStringList nameList;
//    foreach (const AudioMetaObject &o, m_trackList) {
//        if (nameList.contains (o.albumMeta ().name ()))
//            continue;
//        nameList.append (o.albumMeta ().name ());
//    }
//    foreach (const QString &name, nameList) {
//        AudioMetaGroupObject g;
//        g.setName (name);
////        QUrl uri;
//        QMap<QUrl, int> uriMap;
//        AudioMetaList ll;
//        foreach (const AudioMetaObject &o, m_trackList) {
//            if (o.albumMeta ().name () == name) {
//                ll.append (o);
//                QUrl u = o.albumMeta().imgUri();
//                if (u.isValid()) {
//                    if (uriMap.contains(u)) {
//                        int i = uriMap.value(u);
//                        uriMap.insert(u, ++i);
//                    }
//                }
//            }
//        }
//        //fallback
//        if (!uri.isValid () && !ll.isEmpty ()) {
//            foreach (AudioMetaObject o, ll) {
//                uri = o.queryImgUri ();
//                if (uri.isValid ()) {
//                    break;
//                }
//            }
//        }
//        g.setImageUri (uri);
//        g.setList (ll);
//        list.append (g);
//    }
//    return list;
//    QMap<QString, AudioMetaGroupObject> map;
//    foreach(const AudioMetaObject &o, m_trackList) {
//        const QString albumName = o.albumMeta().name();
//        if (map.contains(albumName)) {
//            AudioMetaGroupObject gb = map.value(albumName);
//            AudioMetaList l = gb.list();
//            l.append(o);
//            gb.setList(l);
//            const QUrl iu = o.albumMeta().imgUri();
//            if (iu.isValid()) {
//                QList<QUrl> imgs = gb.imageUri();
//                imgs.append(iu);
//                gb.setImageUri(imgs);
//            }
//        } else {
//            AudioMetaGroupObject gb;
//            gb.setName(albumName);
//            AudioMetaList l;
//            l.append(o);
//            gb.setList(l);
//            const QUrl iu = o.albumMeta().imgUri();
//            if (iu.isValid()) {
//                QList<QUrl> ll;
//                ll.append(iu);
//                gb.setImageUri(ll);
//            }
//            map.insert(albumName, gb);
//        }
//    }
//    return map.values();
    return m_albumGroupMap.values();
}

AudioMetaGroupList MusicLibraryManager::genreList() const
{
//    if (m_trackList.isEmpty ())
//        return AudioMetaGroupList();
//    AudioMetaGroupList list;
//    QStringList nameList;
//    foreach (AudioMetaObject o, m_trackList) {
//        if (nameList.contains (o.trackMeta ().genre ()))
//            continue;
//        nameList.append (o.trackMeta ().genre ());
//    }
//    foreach (QString name, nameList) {
//        AudioMetaGroupObject g;
//        g.setName (name);
//        QUrl uri;
//        AudioMetaList ll;
//        foreach (AudioMetaObject o, m_trackList) {
//            if (o.trackMeta ().genre () == name) {
//                ll.append (o);
//                if (!uri.isValid ()) {
//                    uri = o.coverMeta ().middleUri ();
//                    if (!uri.isValid ())
//                        uri = o.coverMeta ().largeUri ();
//                    if (!uri.isValid ())
//                        uri = o.coverMeta ().smallUri ();
//                }
//            }
//        }
//        //fallback
//        if (!uri.isValid () && !ll.isEmpty ()) {
//            foreach (AudioMetaObject o, ll) {
//                uri = o.queryImgUri ();
//                if (uri.isValid ())
//                    break;
//            }
//        }
//        g.setImageUri (uri);
//        g.setList (ll);
//        list.append (g);
//    }
//    return list;
    return m_genreGroupMap.values();
}

void MusicLibraryManager::setLike(const QString &hash, bool like)
{
    if (!m_dao) {
        qWarning()<<Q_FUNC_INFO<<"Can't find database, this data will lost if app exit!!!";
    }
    m_likeMap.insert(hash, like);
}

void MusicLibraryManager::setLike(const AudioMetaObject &obj, bool like)
{
    setLike(obj.hash(), like);
}

bool MusicLibraryManager::isLike(const QString &hash)
{
    return m_likeMap.value(hash, false);
}

bool MusicLibraryManager::isLike(const AudioMetaObject &obj)
{
    return isLike(obj.hash());
}

void MusicLibraryManager::addLastPlayedTime(const QString &hash)
{
    if (!m_dao) {
        qWarning()<<Q_FUNC_INFO<<"Can't find database, this data will lost if app exit!!!";
    }
    LastPlayedMeta meta = m_lastPlayedMap.value(hash);
    if (meta.isValid()) {
        meta.setTimestamp(QDateTime::currentSecsSinceEpoch());
        m_lastPlayedMap.insert(hash, meta);
    }
}

void MusicLibraryManager::addLastPlayedTime(const AudioMetaObject &obj)
{
    addLastPlayedTime(obj.hash());
}

qint64 MusicLibraryManager::getLastPlayedTime(const QString &hash)
{
    return m_lastPlayedMap.value(hash).timestamp();
}

qint64 MusicLibraryManager::getLastPlayedTime(const AudioMetaObject &obj)
{
    return getLastPlayedTime(obj.hash());
}

void MusicLibraryManager::setPlayedCount(const QString &hash, int count)
{
    if (!m_dao) {
        qWarning()<<Q_FUNC_INFO<<"Can't find database, this data will lost if app exit!!!";
    }
    m_playCntMap.insert(hash, count);
}

void MusicLibraryManager::setPlayedCount(const AudioMetaObject &obj, int count)
{
    return setPlayedCount(obj.hash(), count);
}

void MusicLibraryManager::addPlayedCount(const QString &hash)
{
    int cnt = playedCount(hash);
    setPlayedCount(hash, ++cnt);
}

void MusicLibraryManager::addPlayedCount(const AudioMetaObject &obj)
{
    int cnt = playedCount(obj);
    setPlayedCount(obj, ++cnt);
}

int MusicLibraryManager::playedCount(const QString &hash) const
{
    if (!m_dao) {
        return false;
    }
    return m_dao->playedCount(hash);
}

int MusicLibraryManager::playedCount(const AudioMetaObject &obj) const
{
    return playedCount(obj.hash());
}

LastPlayedMeta MusicLibraryManager::getLastPlayedMeta(const QString &hash) const
{
    if (!m_dao) {
        return LastPlayedMeta();
    }
    return m_dao->getLastPlayedMeta(hash);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedMeta(int limit, bool orderByDesc) const
{
    if (!m_dao) {
        return QList<LastPlayedMeta>();
    }
    return m_dao->getLastPlayedMeta(limit, orderByDesc);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedByAlbum(int limit, bool orderByDesc) const
{
    if (!m_dao) {
        return QList<LastPlayedMeta>();
    }
    return m_dao->getLastPlayedByAlbum(limit, orderByDesc);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedByArtist(int limit, bool orderByDesc) const
{
    if (!m_dao) {
        return QList<LastPlayedMeta>();
    }
    return m_dao->getLastPlayedByArtist(limit, orderByDesc);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedByGenres(int limit, bool orderByDesc) const
{
    if (!m_dao) {
        return QList<LastPlayedMeta>();
    }
    return m_dao->getLastPlayedByGenres(limit, orderByDesc);
}

QStringList MusicLibraryManager::trackHashListByPlayedCount(bool orderByDesc) const
{
    if (!m_dao) {
        return QStringList();
    }
    return m_dao->trackHashListByPlayedCount(orderByDesc);
}

QStringList MusicLibraryManager::trackHashListByLastPlayedTime(bool orderByDesc) const
{
    if (!m_dao) {
        return QStringList();
    }
    return m_dao->trackHashListByLastPlayedTime(orderByDesc);
}

void MusicLibraryManager::saveToDB()
{
    if (m_dao) {
        m_dao->beginTransaction();
        {
            auto i = m_playCntMap.constBegin();
            while (i != m_playCntMap.constEnd()) {
                m_dao->setPlayedCount(i.key(), i.value());
                ++i;
            }
        }
        {
            auto i = m_likeMap.constBegin();
            while (i != m_likeMap.constEnd()) {
                m_dao->setLike(i.key(), i.value());
                ++i;
            }
        }
        {
            auto i = m_lastPlayedMap.constBegin();
            while (i != m_lastPlayedMap.constEnd()) {
                m_dao->setLastPlayedTime(i.value());
                ++i;
            }
        }
        m_dao->commitTransaction();
        m_dao = nullptr;
    }
}

void MusicLibraryManager::initList()
{
    if (!m_trackList.isEmpty()) {
        m_trackList.clear();
    }
    if (!m_dao)
        return;
    QStringList list = m_dao->trackHashList();
    foreach (const QString &hash, list) {
        AudioMetaObject d = m_dao->trackFromHash(hash);
        if (d.isEmpty()) {
            continue;
        }
        m_trackList.append(d);
        m_likeMap.insert(hash, m_dao->isLike(hash));
        m_playCntMap.insert(hash, m_dao->playedCount(hash));
        m_lastPlayedMap.insert(hash, m_dao->getLastPlayedMeta(hash));
        insertToAlbumGroupMap(d);
        insertToArtistGroupMap(d);
        insertToGenreGroupMap(d);
    }
}

void MusicLibraryManager::insertToAlbumGroupMap(const AudioMetaObject &d)
{
    const QString albumName = d.albumMeta().name();
    if (m_albumGroupMap.contains(albumName)) {
        AudioMetaGroupObject gb = m_albumGroupMap.value(albumName);
        AudioMetaList l = gb.list();
        l.append(d);
        gb.setList(l);
        const QUrl iu = d.albumMeta().imgUri();
        if (iu.isValid()) {
            QList<QUrl> imgs = gb.imageUri();
            imgs.append(iu);
            gb.setImageUri(imgs);
        }
    } else {
        AudioMetaGroupObject gb;
        gb.setName(albumName);
        AudioMetaList l;
        l.append(d);
        gb.setList(l);
        const QUrl iu = d.albumMeta().imgUri();
        if (iu.isValid()) {
            QList<QUrl> ll;
            ll.append(iu);
            gb.setImageUri(ll);
        }
        m_albumGroupMap.insert(albumName, gb);
    }
}

void MusicLibraryManager::insertToArtistGroupMap(const AudioMetaObject &d)
{
    const QString artistName = d.artistMeta().name();
    if (m_artistGroupMap.contains(artistName)) {
        AudioMetaGroupObject gb = m_artistGroupMap.value(artistName);
        AudioMetaList l = gb.list();
        l.append(d);
        gb.setList(l);
        const QUrl iu = d.artistMeta().imgUri();
        if (iu.isValid()) {
            QList<QUrl> imgs = gb.imageUri();
            imgs.append(iu);
            gb.setImageUri(imgs);
        }
    } else {
        AudioMetaGroupObject gb;
        gb.setName(artistName);
        AudioMetaList l;
        l.append(d);
        gb.setList(l);
        const QUrl iu = d.artistMeta().imgUri();
        if (iu.isValid()) {
            QList<QUrl> ll;
            ll.append(iu);
            gb.setImageUri(ll);
        }
        m_artistGroupMap.insert(artistName, gb);
    }
}

void MusicLibraryManager::insertToGenreGroupMap(const AudioMetaObject &d)
{
    const QString genreName = d.trackMeta().genre();
    if (m_genreGroupMap.contains(genreName)) {
        AudioMetaGroupObject gb = m_genreGroupMap.value(genreName);
        AudioMetaList l = gb.list();
        l.append(d);
        gb.setList(l);
        const QUrl iu = d.queryImgUri();
        if (iu.isValid()) {
            QList<QUrl> imgs = gb.imageUri();
            imgs.append(iu);
            gb.setImageUri(imgs);
        }
    } else {
        AudioMetaGroupObject gb;
        gb.setName(genreName);
        AudioMetaList l;
        l.append(d);
        gb.setList(l);
        const QUrl iu = d.queryImgUri();
        if (iu.isValid()) {
            QList<QUrl> ll;
            ll.append(iu);
            gb.setImageUri(ll);
        }
        m_genreGroupMap.insert(genreName, gb);
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

