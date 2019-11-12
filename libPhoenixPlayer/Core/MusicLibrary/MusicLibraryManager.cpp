
#include "MusicLibrary/MusicLibraryManager.h"

#include <QThread>
#include <QDebug>
#include <QPointer>
#include <QMutex>
#include <QDateTime>

#include "MusicLibrary/IMusicLibraryDAO.h"
#include "MusicLibrary/LocalMusicScanner.h"
#include "PPSettings.h"
#include "PPUtility.h"
#include "AudioMetaObject.h"
#include "PPCommon.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "LibPhoenixPlayerMain.h"

#include "private/MusicLibraryManagerInternal.h"
#include "private/SingletonObjectFactory.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : BaseObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->musicLibraryManagerInternal();

    connect(m_internal.data(), &MusicLibraryManagerInternal::libraryListSizeChanged,
            this, &MusicLibraryManager::libraryListSizeChanged);
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";

    saveToDB();

    m_internal->disconnect(this);

    qDebug()<<">>>>>>>> after "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";
}

AudioMetaList MusicLibraryManager::allTracks() const
{
    return m_internal->tracks();
}

AudioMetaObject MusicLibraryManager::trackFromHash(const QString &hash) const
{
    if (!m_internal->daoValid()) {
        return AudioMetaObject();
    }
    return m_internal->dao()->trackFromHash(hash);
}

bool MusicLibraryManager::empty() const
{
    return m_internal->trackList()->isEmpty ();
}

void MusicLibraryManager::deleteObject(const AudioMetaObject &obj, bool deleteFromLocalDisk)
{
    if (m_internal->daoValid()) {
        return;
    }
    if (m_internal->dao()->deleteMetaData(obj) && deleteFromLocalDisk) {
        QString file = QString("%1/%2").arg(obj.path()).arg(obj.name());
        if (QFile::exists(file)) {
            if (!QFile::remove(file)) {
                qWarning()<<Q_FUNC_INFO<<"remove failure file ["<<file<<"]";
            }
        }
    }
}

void MusicLibraryManager::deleteObject(const QString &hash, bool deleteFromLocalDisk)
{
    AudioMetaObject obj = trackFromHash(hash);
    if (obj.isHashEmpty()) {
        return;
    }
    deleteObject(obj, deleteFromLocalDisk);
}

#define GET_LIST(ca, cb, limitNum) \
    if (m_internal->trackList()->isEmpty()) { \
        return AudioMetaList(); \
    } \
    AudioMetaList list; \
    int i = 0; \
    for (auto d = m_internal->trackList()->constBegin(); d != m_internal->trackList()->constEnd(); ++d) { \
        if (d->ca == cb) { \
            list.append(*d); \
        } \
        i++; \
        if (limitNum > 0 && i >= limitNum) { \
            break; \
        } \
    } \
    return list; \

AudioMetaList MusicLibraryManager::artistTracks(const QString &artistName, int limitNum)
{
    GET_LIST(artistMeta().name(), artistName, limitNum)
}

AudioMetaList MusicLibraryManager::albumTracks(const QString &albumName, int limitNum)
{
    GET_LIST(albumMeta().name(), albumName, limitNum)
}

AudioMetaList MusicLibraryManager::genreTracks(const QString &genreName, int limitNum)
{
    GET_LIST(trackMeta().genre(), genreName, limitNum)
}

AudioMetaList MusicLibraryManager::mediaTypeTracks(const QString &mediaType, int limitNum)
{
    GET_LIST(mediaType(), mediaType.toInt(), limitNum)
}

AudioMetaList MusicLibraryManager::userRatingTracks(const QString &rating, int limitNum)
{
    GET_LIST(trackMeta().userRating(), rating, limitNum)
}

AudioMetaList MusicLibraryManager::folderTracks(const QString &folder, int limitNum)
{
    GET_LIST(path(), folder, limitNum)
}

AudioMetaGroupList MusicLibraryManager::artistList() const
{
    return m_internal->artistGroupMap()->values();
}

AudioMetaGroupList MusicLibraryManager::albumList() const
{
    return m_internal->albumGroupMap()->values();
}

AudioMetaGroupList MusicLibraryManager::genreList() const
{
    return m_internal->genreGroupMap()->values();
}

void MusicLibraryManager::setLike(const QString &hash, bool like)
{
    if (m_internal->daoValid()) {
        qWarning()<<Q_FUNC_INFO<<"Can't find database, this data will lost if app exit!!!";
    }
    m_internal->likeMap()->insert(hash, like);
}

void MusicLibraryManager::setLike(const AudioMetaObject &obj, bool like)
{
    setLike(obj.hash(), like);
}

bool MusicLibraryManager::isLike(const QString &hash)
{
    return m_internal->likeMap()->value(hash, false);
}

bool MusicLibraryManager::isLike(const AudioMetaObject &obj)
{
    return isLike(obj.hash());
}

void MusicLibraryManager::addLastPlayedTime(const QString &hash)
{
    m_internal->addLastPlayedTime(hash);
}

void MusicLibraryManager::addLastPlayedTime(const AudioMetaObject &obj)
{
    addLastPlayedTime(obj.hash());
}

qint64 MusicLibraryManager::getLastPlayedTime(const QString &hash)
{
    return m_internal->lastPlayedMap()->value(hash).timestamp();
}

qint64 MusicLibraryManager::getLastPlayedTime(const AudioMetaObject &obj)
{
    return getLastPlayedTime(obj.hash());
}

void MusicLibraryManager::setPlayedCount(const QString &hash, int count)
{
    m_internal->setPlayedCount(hash, count);
}

void MusicLibraryManager::setPlayedCount(const AudioMetaObject &obj, int count)
{
    setPlayedCount(obj.hash(), count);
}

void MusicLibraryManager::addPlayedCount(const QString &hash)
{
    m_internal->addPlayedCount(hash);
}

void MusicLibraryManager::addPlayedCount(const AudioMetaObject &obj)
{
    m_internal->addPlayedCount(obj);
}

int MusicLibraryManager::playedCount(const QString &hash) const
{
    return m_internal->playedCount(hash);
}

int MusicLibraryManager::playedCount(const AudioMetaObject &obj) const
{
    return playedCount(obj.hash());
}

LastPlayedMeta MusicLibraryManager::getLastPlayedMeta(const QString &hash) const
{
    if (m_internal->daoValid()) {
        return LastPlayedMeta();
    }
    return m_internal->dao()->getLastPlayedMeta(hash);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedMeta(int limit, bool orderByDesc) const
{
    if (m_internal->daoValid()) {
        return QList<LastPlayedMeta>();
    }
    return m_internal->dao()->getLastPlayedMeta(limit, orderByDesc);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedByAlbum(int limit, bool orderByDesc) const
{
    if (m_internal->daoValid()) {
        return QList<LastPlayedMeta>();
    }
    return m_internal->dao()->getLastPlayedByAlbum(limit, orderByDesc);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedByArtist(int limit, bool orderByDesc) const
{
    if (m_internal->daoValid()) {
        return QList<LastPlayedMeta>();
    }
    return m_internal->dao()->getLastPlayedByArtist(limit, orderByDesc);
}

QList<LastPlayedMeta> MusicLibraryManager::getLastPlayedByGenres(int limit, bool orderByDesc) const
{
    if (m_internal->daoValid()) {
        return QList<LastPlayedMeta>();
    }
    return m_internal->dao()->getLastPlayedByGenres(limit, orderByDesc);
}

QStringList MusicLibraryManager::trackHashListByPlayedCount(bool orderByDesc) const
{
    if (m_internal->daoValid()) {
        return QStringList();
    }
    return m_internal->dao()->trackHashListByPlayedCount(orderByDesc);
}

QStringList MusicLibraryManager::trackHashListByLastPlayedTime(bool orderByDesc) const
{
    if (m_internal->daoValid()) {
        return QStringList();
    }
    return m_internal->dao()->trackHashListByLastPlayedTime(orderByDesc);
}

void MusicLibraryManager::saveToDB()
{
    if (m_internal->daoValid()) {
        m_internal->dao()->beginTransaction();
        {
            auto i = m_internal->playCntMap()->constBegin();
            while (i != m_internal->playCntMap()->constEnd()) {
                m_internal->dao()->setPlayedCount(i.key(), i.value());
                ++i;
            }
        }
        {
            auto i = m_internal->likeMap()->constBegin();
            while (i != m_internal->likeMap()->constEnd()) {
                m_internal->dao()->setLike(i.key(), i.value());
                ++i;
            }
        }
        {
            auto i = m_internal->lastPlayedMap()->constBegin();
            while (i != m_internal->lastPlayedMap()->constEnd()) {
                m_internal->dao()->setLastPlayedTime(i.value());
                ++i;
            }
        }
        m_internal->dao()->commitTransaction();
    }
}

QList<QList<qreal> > MusicLibraryManager::loadSpectrumData(const AudioMetaObject &obj)
{
    if (obj.isHashEmpty() || m_internal->daoValid()) {
        return QList<QList<qreal>>();
    }
    return m_internal->dao()->getSpectrumData(obj);
}

} //MusicLibrary
} //PhoenixPlayer

