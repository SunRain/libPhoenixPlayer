
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

namespace PhoenixPlayer {
namespace MusicLibrary {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : QObject(parent)
    ,isInit(false)
{
    mDiskLooKupThread = nullptr;
    mDiskLooKup = nullptr;
    mPlayListDAO = 0;
    mTagParserManager = nullptr;
    mTagParserThread = nullptr;

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mSettings = Settings::instance();
    mPluginLoader = PluginLoader::instance();
#else
    qDebug()<<"For other os";
    mSettings = SingletonPointer<Settings>::instance ();
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
    mCurrentSongHash = QString();
    mCurrentPlayListHash = QString();

    if (!isInit)
        init ();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
    if (mSettings != nullptr) {
        qDebug()<<"save Settings";
        mSettings->setLastPlayedSong (mCurrentSongHash);
    }

    if (mDiskLooKup != nullptr) {
        mDiskLooKup->stopLookup ();
    }

    if (mDiskLooKupThread != nullptr) {
        qDebug()<<"wait for DiskLooKupThread";
        mDiskLooKupThread->wait (3 * 60 * 1000);
        mDiskLooKupThread->quit();
    }
    mDiskLooKupThread->deleteLater ();

    if (mTagParserThread != nullptr) {
        qDebug()<<"wait for TagParserThread";
        mTagParserThread->wait (3 * 60 * 1000);
        mTagParserThread->quit();
    }

    qDebug()<<"after delete thread";

    if (!mPlayListDAO.isNull())
        mPlayListDAO.data()->deleteLater ();

    qDebug()<<">>>>>>>> after "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
}
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
MusicLibraryManager *MusicLibraryManager::instance()
{
    qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
    static QMutex mutex;
    static QScopedPointer<MusicLibraryManager> scp;
    if (Q_UNLIKELY(scp.isNull())) {
        qDebug()<<">>>>>>>> statr new";
        mutex.lock();
        scp.reset(new MusicLibraryManager(0));
        mutex.unlock();
    }
    qDebug()<<">>>>>>>> return "<<scp.data()->metaObject()->className();;
    return scp.data();
}
#endif

bool MusicLibraryManager::scanLocalMusic()
{
    qDebug()<<__FUNCTION__;
    //本地歌曲扫描线程
    if (mDiskLooKupThread == nullptr) {
        qDebug()<<__FUNCTION__<<" new mDiskLooKupThread";
        mDiskLooKupThread = new QThread(0);
    }
    if (mDiskLooKup == nullptr) {
        qDebug()<<__FUNCTION__<<" new mDiskLooKup";

        mDiskLooKup = new DiskLookup(0);
        mDiskLooKup->moveToThread (mDiskLooKupThread);

        connect (mDiskLooKupThread, &QThread::started, [this] {
            qDebug()<<__FUNCTION__<<" Thread start, we'll start lookup now";
            mDiskLooKup->startLookup();
        });

//        //use Transaction in Sailfish OS will make app hang when trying to write large data
//#ifndef SAILFISH_OS
        connect (mDiskLooKup, &DiskLookup::pending,
                 mPlayListDAO.data(), &IPlayListDAO::beginTransaction);
//#endif

        connect (mDiskLooKup, &DiskLookup::finished,
                 [this] {
            qDebug()<<__FUNCTION__<<" DiskLookup finished, we'll try to finish thread";
            mDiskLooKupThread->quit();

            if (mTagParserManager == nullptr || mTagParserThread == nullptr)
                initTagParserManager();
            mTagParserThread->start();
//            emit searchingFinished ();
        });

        connect (mDiskLooKupThread, &QThread::finished, [this] {
            qDebug()<<__FUNCTION__<<" mDiskLooKupThread finished, we'll try to deleteLater";
            mDiskLooKup->deleteLater();
            mDiskLooKupThread->deleteLater();
        });

        connect (mDiskLooKup, &DiskLookup::fileFound,
                 [this]
                 (const QString &path, const QString &file, const qint64 &size) {
            QString hash = Util::calculateHash (path.toLocal8Bit ()
                                                + file.toLocal8Bit ()
                                                + QString::number (size));

            qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;

            emit searching (path, file, size);

            PhoenixPlayer::SongMetaData *data = new PhoenixPlayer::SongMetaData(0);
            data->setMeta (Common::SongMetaTags::E_FileName, file);
            data->setMeta (Common::SongMetaTags::E_FilePath, path);
            data->setMeta (Common::SongMetaTags::E_Hash, hash);
            data->setMeta (Common::SongMetaTags::E_FileSize, size);
            if (mTagParserManager == nullptr || mTagParserThread == nullptr)
                initTagParserManager();
            mTagParserManager->addItem (data, false);
        });
    }
    foreach (QString s, mSettings->getMusicDirs ()) {
        mDiskLooKup->addLookupDir (s, false);
    }
    //return mDiskLooKup.data ()->startLookup ();
    mDiskLooKupThread->start();
    return true;
}

bool MusicLibraryManager::changePlayList(const QString &playListHash)
{
    if (mCurrentPlayListHash == playListHash) {
        qDebug()<<"Same playList, not changed";
        return false;
    }
    mCurrentPlayListHash = playListHash;
    mCurrentSongHash = QString();
    emit playListChanged ();
    return true;
}

bool MusicLibraryManager::createPlayList(const QString &playListName)
{
    return mPlayListDAO.data()->insertPlayList (playListName);
}

bool MusicLibraryManager::deletePlayList(const QString &playListHash)
{
    return mPlayListDAO.data()->deletePlayList (playListHash);
}

QString MusicLibraryManager::playingSongHash()
{
    if (mCurrentSongHash.isEmpty ()) {
        qDebug()<<"try playingSongHash from settings";
        mCurrentSongHash = mSettings->getLastPlayedSong ();
        if (mCurrentSongHash.isEmpty ()
                && !mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash).isEmpty ()) {
            qDebug()<<"try playingSongHash from first from library";
            mCurrentSongHash = mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash).first ();
        } else {
            qDebug()<<__FUNCTION__<<"get some error";
        }
    }
    qDebug()<<"playingSongHash is "<<mCurrentSongHash;
    return mCurrentSongHash;
}

void MusicLibraryManager::setPlayingSongHash(const QString &newHash)
{
    if (newHash.isEmpty ())
        return;
    if (mCurrentSongHash == newHash)
        return;
    qDebug()<<__FUNCTION__<<"change current song hash from "<<mCurrentSongHash<<" to "<<newHash;
    mCurrentSongHash = newHash;
    emit playingSongChanged ();
}

QString MusicLibraryManager::firstSongHash()
{
    mCurrentSongHash = mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash).first ();
    return mCurrentSongHash;
}

QString MusicLibraryManager::lastSongHash()
{
    mCurrentSongHash = mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash).last ();
    return mCurrentSongHash;
}

void MusicLibraryManager::nextSong()
{
    QStringList list = mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash);
    int index = list.indexOf (mCurrentSongHash) +1;
    if (index >= list.size ())
        index = 0;
    mCurrentSongHash = list.at (index);
    emit playingSongChanged ();
}

void MusicLibraryManager::preSong()
{
    QStringList list = mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash);
    int index = list.indexOf (mCurrentSongHash);
    if (index == -1) { //no hash found
        index = 0;
    } else if (index == 0) { //hash is the first song
        index = list.size () -1; //jump to last song
    } else {
        index --;
    }
    mCurrentSongHash = list.at (index);
    emit playingSongChanged ();
}

void MusicLibraryManager::randomSong()
{
    QTime time = QTime::currentTime ();
    qsrand(time.second () * 1000 + time.msec ());
    int n = qrand ();
    n = n % mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash).size ();
    mCurrentSongHash = mPlayListDAO.data()->getSongHashList (mCurrentPlayListHash).at (n);
    emit playingSongChanged ();
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
    return mPlayListDAO.data()->queryMusicLibrary(targetColumn, regColumn, regValue, skipDuplicates);
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
        list = mPlayListDAO.data()
                ->queryMusicLibrary (targetColumn,
                                     Common::SongMetaTags::E_FirstFlag, //UnUsed
                                     QString(),
                                     skipDuplicates);
    } else {
        list = mPlayListDAO.data()
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
        list = mPlayListDAO.data()
                ->queryPlayList (targetColumn,
                                 Common::PlayListFirstFlag,
                                 QString());
    } else {
        list = mPlayListDAO.data()
                ->queryPlayList (targetColumn, Common::PlayListHash, hash);
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
    return mPlayListDAO.data()
            ->updatePlayList (Common::PlayListSongHashes,
                              playListHash,
                              newSongHash,
                              true);
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
    return mPlayListDAO.data()
            ->updatePlayList (Common::PlayListSongHashes,
                              playListHash,
                              songHash,
                              false);
}

bool MusicLibraryManager::init()
{
    qDebug()<<__FUNCTION__;

    mCurrentSongHash = mSettings->getLastPlayedSong ();
    mCurrentPlayListHash = mSettings->getPlayListHash ();

    if (mPlayListDAO.isNull())
        mPlayListDAO = mPluginLoader->getCurrentPlayListDAO ();
    if (!mPlayListDAO.isNull()) {
        if (!mPlayListDAO.data()->initDataBase ()) {
            qDebug()<<"initDataBase error";
        }
    } else {
        qDebug()<<"Can't find mPlayListDAO";
    }
    isInit = true;

    return true;
}

void MusicLibraryManager::initTagParserManager()
{
    //歌曲tag读取线程
    if (mTagParserThread == nullptr)
        mTagParserThread = new QThread(0);
    if (mTagParserManager == nullptr) {
        mTagParserManager = new TagParserManager(0);
        mTagParserManager->moveToThread (mTagParserThread);

        connect (mTagParserManager, &TagParserManager::parserPending, [] {
            qDebug()<<"********************* parserPending";
        });

        connect (mTagParserThread, &QThread::started, [this] () {
            qDebug()<<__FUNCTION__<<" mTagParserThread started, start parser tag";
            mTagParserManager->startParserLoop();
        });

        connect (mTagParserManager, &TagParserManager::parserQueueFinished, [this] () {
            qDebug()<<__FUNCTION__<<" mTagParserManager finished, stop thread";
            mTagParserThread->quit();
//            //use Transaction in Sailfish OS will make app hang when trying to write large data
//#ifndef SAILFISH_OS
            mPlayListDAO.data()->commitTransaction ();
//#endif
            emit searchingFinished ();
        });

        connect (mTagParserThread, &QThread::finished, [this] {
            qDebug()<<__FUNCTION__<<" mTagParserThread finished, we'll try to deleteLater";
            mTagParserManager->deleteLater();
            mTagParserThread->deleteLater();
        });
    }
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

