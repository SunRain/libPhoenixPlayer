
#include "MusicLibrary/MusicLibraryManager.h"

#include <QThread>
#include <QDebug>
#include <QPointer>
#include <QMutex>

#include "MusicLibrary/IPlayListDAO.h"
#include "DiskLookup.h"
#include "Settings.h"
#include "Util.h"
#include "SongMetaData.h"
#include "Common.h"
#include "TagParserManager.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "AsyncDiskLookup.h"
#include "AsyncTagParserMgrWrapper.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : QObject(parent)
    ,m_isInit(false)
{
    m_asyncDiskLookup = nullptr;
    m_tagParserWrapper = nullptr;
    m_playListDAO = nullptr;

    m_settings = Settings::instance ();
    m_pluginLoader = PluginLoader::instance ();

    m_currentSongHash = QString();
    m_currentPlayListHash = QString();

    if (!m_isInit)
        init ();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";
    if (m_settings != nullptr) {
        qDebug()<<"save Settings";
        m_settings->setLastPlayedSong (m_currentSongHash);
    }

//    if (m_playListDAO)
//        m_playListDAO->deleteLater ();

    qDebug()<<">>>>>>>> after "<< Q_FUNC_INFO <<" <<<<<<<<<<<<<<<<";
}
//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//MusicLibraryManager *MusicLibraryManager::instance()
//{
//    static QMutex mutex;
//    static QScopedPointer<MusicLibraryManager> scp;
//    if (Q_UNLIKELY(scp.isNull())) {
//        qDebug()<<Q_FUNC_INFO<<">>>>>>>> statr new";
//        mutex.lock();
//        scp.reset(new MusicLibraryManager(0));
//        mutex.unlock();
//    }
//    return scp.data();
//}
//#endif

bool MusicLibraryManager::changePlayList(const QString &playListHash)
{
    if (m_currentPlayListHash == playListHash) {
        qDebug()<<"Same playList, not changed";
        return false;
    }
    m_currentPlayListHash = playListHash;
    m_currentSongHash = QString();
    emit playListChanged ();
    return true;
}

bool MusicLibraryManager::createPlayList(const QString &playListName)
{
    if (m_playListDAO->insertPlayList (playListName)) {
        emit playListCreated ();
        return true;
    }
    return false;
}

bool MusicLibraryManager::deletePlayList(const QString &playListHash)
{
    if (m_playListDAO->deletePlayList (playListHash)) {
        emit playListDeleted ();
        return true;
    }
    return false;
}

QString MusicLibraryManager::getCurrentPlayListHash()
{
    return m_currentPlayListHash;
}

QString MusicLibraryManager::playingSongHash()
{
    if (m_currentSongHash.isEmpty ()) {
        qDebug()<<"try playingSongHash from settings";
        m_currentSongHash = m_settings->getLastPlayedSong ();
        if (m_currentSongHash.isEmpty ()
                && !m_playListDAO->getSongHashList (m_currentPlayListHash).isEmpty ()) {
            qDebug()<<"try playingSongHash from first from library";
            m_currentSongHash = m_playListDAO->getSongHashList (m_currentPlayListHash).first ();
        } else {
            qDebug()<<Q_FUNC_INFO<<"get some error";
        }
    }
    qDebug()<<"playingSongHash is "<<m_currentSongHash;
    return m_currentSongHash;
}

void MusicLibraryManager::setPlayingSongHash(const QString &newHash)
{
    if (newHash.isEmpty ())
        return;
    if (m_currentSongHash == newHash)
        return;
    qDebug()<<Q_FUNC_INFO<<"change current song hash from "<<m_currentSongHash<<" to "<<newHash;
    m_currentSongHash = newHash;
    emit playingSongChanged ();
}

QString MusicLibraryManager::firstSongHash()
{
    m_currentSongHash = m_playListDAO->getSongHashList (m_currentPlayListHash).first ();
    return m_currentSongHash;
}

QString MusicLibraryManager::lastSongHash()
{
    m_currentSongHash = m_playListDAO->getSongHashList (m_currentPlayListHash).last ();
    return m_currentSongHash;
}

QString MusicLibraryManager::nextSong(bool jumpToNextSong)
{
    QStringList list = m_playListDAO->getSongHashList (m_currentPlayListHash);
    if (!list.isEmpty ()) {
        int index = list.indexOf (m_currentSongHash) +1;
        if (index >= list.size ())
            index = 0;
        if (jumpToNextSong) {
            m_currentSongHash = list.at (index);
            emit playingSongChanged ();
        }
        return list.at (index);
    }
    return QString();
}

QString MusicLibraryManager::preSong(bool jumpToPreSong)
{
    QStringList list = m_playListDAO->getSongHashList (m_currentPlayListHash);
    if (!list.isEmpty ()) {
        int index = list.indexOf (m_currentSongHash);
        if (index == -1) { //no hash found
            index = 0;
        } else if (index == 0) { //hash is the first song
            index = list.size () -1; //jump to last song
        } else {
            index --;
        }
        if (jumpToPreSong) {
            m_currentSongHash = list.at (index);
            emit playingSongChanged ();
        }
        return list.at (index);
    }
    return QString();
}

QString MusicLibraryManager::randomSong(bool jumpToRandomSong)
{
    QStringList list = m_playListDAO->getSongHashList (m_currentPlayListHash);
    if (!list.isEmpty ()) {
        QTime time = QTime::currentTime ();
        qsrand(time.second () * 1000 + time.msec ());
        int n = qrand ();
        n = n % list.size ();
        if (jumpToRandomSong) {
            m_currentSongHash = list.at (n);
            emit playingSongChanged ();
        }
        return list.at (n);
    }
    return QString();
}

QString MusicLibraryManager::queryOneByIndex(const QString &hash, int tag, bool skipDuplicates)
{
    return queryOne(hash, Common::SongMetaTags(tag), skipDuplicates);
}

QStringList MusicLibraryManager::queryMusicLibrary(Common::SongMetaTags targetColumn,
                                                   Common::SongMetaTags regColumn,
                                                   const QString &regValue,
                                                   bool skipDuplicates)
{
    return m_playListDAO->queryMusicLibrary(targetColumn, regColumn, regValue, skipDuplicates);
}

QString MusicLibraryManager::querySongImageUri(const QString &hash)
{
    QString uri = queryOne(hash, Common::E_CoverArtMiddle);
    if (uri.isEmpty())
        uri = queryOne(hash, Common::E_CoverArtLarge);
    if (uri.isEmpty())
        uri = queryOne(hash, Common::E_CoverArtSmall);
    if (uri.isEmpty())
        uri = queryOne(hash, Common::E_AlbumImageUrl);
    if (uri.isEmpty())
        uri = queryOne(hash, Common::E_ArtistImageUri);
    return uri;
}

QString MusicLibraryManager::querySongTitle(const QString &hash)
{
    QString str = queryOne(hash, Common::E_SongTitle);
    if (str.isEmpty()) {
        str = queryOne(hash, Common::E_FileName);
        if (!str.isEmpty())
            str = str.mid(0, str.lastIndexOf("."));
    }
    return str;
}

QStringList MusicLibraryManager::querySongMetaElement(Common::SongMetaTags targetColumn,
                                                      const QString &hash,
                                                      bool skipDuplicates)
{
    QStringList list;
    if (hash.isEmpty ()) {
        list = m_playListDAO
                ->queryMusicLibrary (targetColumn,
                                     Common::SongMetaTags::E_FirstFlag, //UnUsed
                                     QString(),
                                     skipDuplicates);
    } else {
        list = m_playListDAO
                ->queryMusicLibrary (targetColumn,
                                     Common::SongMetaTags::E_Hash,
                                     hash,
                                     skipDuplicates);
    }
    return list;
}

QStringList MusicLibraryManager::querySongMetaElementByIndex(
        int columnIndex, const QString &hash, bool skipDuplicates)
{
    return querySongMetaElement (Common::SongMetaTags(columnIndex), hash, skipDuplicates);
}

QStringList MusicLibraryManager::queryPlayListElement(
        Common::PlayListElement targetColumn, const QString &hash)
{
    QStringList list;
    if (hash.isEmpty ()) {
        list = m_playListDAO->queryPlayList (targetColumn, Common::PlayListFirstFlag, QString());
    } else {
        list = m_playListDAO->queryPlayList (targetColumn, Common::PlayListHash, hash);
    }

    qDebug()<<" query result "<< list;
    return list;
}

QStringList MusicLibraryManager::queryPlayListElementByIndex(int index, const QString &hash)
{
    return queryPlayListElement (Common::PlayListElement(index), hash);
}

bool MusicLibraryManager::insertToPlayList(const QString &playListHash,
                                           const QString &newSongHash)
{
    if (playListHash.isEmpty () || newSongHash.isEmpty ()) {
        qDebug()<<"playListHash or newSongHash is empty";
        return false;
    }
    bool ret = m_playListDAO->updatePlayList (Common::PlayListSongHashes,
                                                    playListHash,
                                                    newSongHash,
                                                    true);
    if (ret)
        emit playListTrackChanged ();
    return ret;
}

bool MusicLibraryManager::deleteFromPlayList(
        const QString &playListHash, const QString &songHash, bool deleteFromStorage)
{
    Q_UNUSED(deleteFromStorage)

    if (playListHash.isEmpty () || songHash.isEmpty ()) {
        qDebug()<<"playListHash or newSongHash is empty";
        return false;
    }
    //TODO: 需要添加从本地删除的功能
    bool ret = m_playListDAO->updatePlayList (Common::PlayListSongHashes,
                                                    playListHash,
                                                    songHash,
                                                    false);
    if (ret)
        emit playListTrackChanged ();
    return ret;
}

bool MusicLibraryManager::init()
{
    qDebug()<<QString(">>>>>>>>>> %1 <<<<<<<<<").arg (Q_FUNC_INFO);

    m_currentSongHash = m_settings->getLastPlayedSong ();
    m_currentPlayListHash = m_settings->getPlayListHash ();

    if (!m_playListDAO)
        m_playListDAO = m_pluginLoader->getCurrentPlayListDAO ();
    if (m_playListDAO) {
        if (!m_playListDAO->initDataBase ()) {
            qDebug()<<"initDataBase error";
        }
    } else {
        qDebug()<<"Can't find mPlayListDAO";
    }
    m_isInit = true;

    return true;
}

QString MusicLibraryManager::queryOne(const QString &hash, Common::SongMetaTags tag, bool skipDuplicates)
{
    if (hash.isEmpty())
        return QString();
    QStringList list = this->querySongMetaElement(tag, hash, skipDuplicates);
    if (list.isEmpty())
        return QString();
    return list.first();
}

} //MusicLibrary
} //PhoenixPlayer

