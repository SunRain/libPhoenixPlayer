#include "MusicLibraryManagerInternal.h"

#include <QDebug>

#include "PluginMgrInternal.h"
#include "PPSettingsInternal.h"

#include "MusicLibrary/LocalMusicScanner.h"
#include "MusicLibrary/IMusicLibraryDAO.h"

namespace PhoenixPlayer {

using namespace MusicLibrary;

MusicLibraryManagerInternal::MusicLibraryManagerInternal(QSharedPointer<PPSettingsInternal> set,
                                                                        QSharedPointer<PluginMgrInternal> loader,
                                                                        QObject *parent)
    : QObject(parent),
    m_settings(set),
    m_pluginMgr(loader)
{
    initDAO();
    initList();
}

MusicLibraryManagerInternal::~MusicLibraryManagerInternal()
{
    if (m_dao && PluginMetaData::isValid(m_usedPluginMeta)) {
        PluginMgr::unload(m_usedPluginMeta);
    }
    if (!m_trackList.isEmpty ()) {
        m_trackList.clear ();
    }
}

void MusicLibraryManagerInternal::addLastPlayedTime(const QString &hash)
{
    if (this->daoValid()) {
        qWarning()<<Q_FUNC_INFO<<"Can't find database, this data will lost if app exit!!!";
    }
    LastPlayedMeta meta = m_lastPlayedMap.value(hash);
    if (meta.isValid()) {
        meta.setTimestamp(QDateTime::currentSecsSinceEpoch());
        m_lastPlayedMap.insert(hash, meta);
    }
}

void MusicLibraryManagerInternal::addLastPlayedTime(const AudioMetaObject &obj)
{
    addLastPlayedTime(obj.hash());
}

void MusicLibraryManagerInternal::addPlayedCount(const QString &hash)
{
    int cnt = playedCount(hash);
    setPlayedCount(hash, ++cnt);
}

void MusicLibraryManagerInternal::addPlayedCount(const AudioMetaObject &obj)
{
    addPlayedCount(obj.hash());
}

int MusicLibraryManagerInternal::playedCount(const QString &hash) const
{
    if (this->daoValid()) {
        return false;
    }
    return m_dao->playedCount(hash);
}

int MusicLibraryManagerInternal::playedCount(const AudioMetaObject &obj) const
{
    return playedCount(obj.hash());
}

void MusicLibraryManagerInternal::setPlayedCount(const QString &hash, int count)
{
    if (this->daoValid()) {
        qWarning()<<Q_FUNC_INFO<<"Can't find database, this data will lost if app exit!!!";
    }
   m_playCntMap.insert(hash, count);
}

void MusicLibraryManagerInternal::setPlayedCount(const AudioMetaObject &obj, int count)
{
    setPlayedCount(obj.hash(), count);
}

void MusicLibraryManagerInternal::initDAO()
{
    m_usedPluginMeta = m_pluginMgr->usedMusicLibraryDAO();
    if (!PluginMetaData::isValid(m_usedPluginMeta)) {
        qWarning()<<Q_FUNC_INFO<<"Can't find MusicLibrary DAO plugin !";
        return;
    }
    m_dao = qobject_cast<MusicLibrary::IMusicLibraryDAO*>(PluginMgr::instance(m_usedPluginMeta));
    if (!m_dao) {
        qWarning()<<Q_FUNC_INFO<<"Can't load DAO plugin !";
        return;
    }

    connect (m_dao, &IMusicLibraryDAO::metaDataInserted, this, [&](const QString &hash) {
        QStringList daoHashList = m_dao->trackHashList();
        QStringList trackList;
        foreach (const AudioMetaObject &d, m_trackList) {
            trackList.append(d.hash());
        }
        foreach (const QString &s, daoHashList) {
            if (!trackList.contains(s)) {
                AudioMetaObject o = m_dao->trackFromHash(s);
                m_trackList.append(o);
                this->insertToAlbumGroupMap(o);
                this->insertToGenreGroupMap(o);
                this->insertToArtistGroupMap(o);
            }
        }
        emit this->libraryListSizeChanged();
    });
    connect (m_dao, &IMusicLibraryDAO::metaDataDeleted, this, [&](const QString &hash) {
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
        emit this->libraryListSizeChanged();
    });


}

void MusicLibraryManagerInternal::initList()
{
    if (!m_trackList.isEmpty()) {
        m_trackList.clear();
    }
    if (!m_dao) {
        return;
    }
    QStringList list = m_dao->trackHashList();
    foreach (const QString &hash, list) {
        AudioMetaObject d = m_dao->trackFromHash(hash);
        if (d.isHashEmpty()) {
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

void MusicLibraryManagerInternal::insertToAlbumGroupMap(const AudioMetaObject &d)
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

void MusicLibraryManagerInternal::insertToArtistGroupMap(const AudioMetaObject &d)
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

void MusicLibraryManagerInternal::insertToGenreGroupMap(const AudioMetaObject &d)
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

} //namespace PhoenixPlayer
