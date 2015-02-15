
#include <QThread>
#include <QDebug>
#include <QPointer>
#include <QMutex>

#include "MusicLibrary/IPlayListDAO.h"
#include "MusicLibraryManager.h"
//#include "PlayListDAOLoader.h"
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
    mDiskLooKupThread = 0;
    mDiskLooKup = 0;
    mPlayListDAO = 0;
    mTagParserManager = 0;
    mTagParserThread = 0;

    qDebug()<<__FUNCTION__;

#ifdef SAILFISH_OS
    qDebug()<<"For Sailfish os";
//    SingletonPointer<Settings> ss;
//    SingletonPointer<PluginLoader> sp;
//    mSettings = ss.instance();
//    mPluginLoader = sp.instance();
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

    if (!mDiskLooKup.isNull ()) {
        mDiskLooKup.data ()->stopLookup ();
        mDiskLooKup.data ()->deleteLater ();
    }

    if (!mDiskLooKupThread.isNull ()) {
        qDebug()<<"wait for DiskLooKupThread";
//        mDiskLooKupThread.data ()->quit ();
        mDiskLooKupThread.data ()->wait (3 * 60 * 1000);
    }
    mDiskLooKupThread->deleteLater ();

    if (!mTagParserThread.isNull ()) {
        qDebug()<<"wait for TagParserThread";
//        mTagParserThread.data ()->quit ();
        mDiskLooKupThread.data ()->wait (3 * 60 * 1000);
    }

    qDebug()<<"after delete thread";

    if (!mPlayListDAO.isNull ())
        mPlayListDAO.data ()->deleteLater ();

    qDebug()<<">>>>>>>> after "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
}
#ifdef SAILFISH_OS
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

//MusicLibraryManager *MusicLibraryManager::getInstance()
//{
//    static MusicLibraryManager m;
//    return &m;
//}

//void MusicLibraryManager::initSettings()
//{
//    mSettings = Settings::getInstance ();
//    mCurrentSongHash = mSettings->getLastPlayedSong ();
//    mCurrentPlayListHash = mSettings->getPlayListHash ();
//}

//void MusicLibraryManager::initPluginLoader()
//{
//    mPluginLoader = PluginLoader::getInstance ();
//}

bool MusicLibraryManager::scanLocalMusic()
{
    //本地歌曲扫描线程
    if (mDiskLooKupThread.isNull ()) {
        mDiskLooKupThread = new QThread(this);

        //signal/slot
        connect (mDiskLooKupThread.data (), &QThread::finished,
                 mDiskLooKup.data (), &DiskLookup::deleteLater);
    }
    if (mDiskLooKup.isNull ()) {
        mDiskLooKup = new DiskLookup(0);
        mDiskLooKup.data ()->moveToThread (mDiskLooKupThread);

        //signal/slot
        connect (mDiskLooKup.data (), &DiskLookup::pending,
                 mPlayListDAO.data (), &IPlayListDAO::beginTransaction);

        connect (mDiskLooKup.data (),
                 &DiskLookup::finished,
                 [this] {
            if (mTagParserManager.isNull())
                initTagParserManager();
            mTagParserManager.data ()->startParserLoop ();
            emit searchingFinished ();
        });

        connect (mDiskLooKup.data (),
                 &DiskLookup::fileFound,
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
            if (mTagParserManager.isNull())
                initTagParserManager();
            mTagParserManager.data ()->addItem (data, false);
        });
    }

//    //signal/slot
//    connect (mDiskLooKupThread.data (), &QThread::finished,
//             mDiskLooKup.data (), &DiskLookup::deleteLater);

//    connect (mDiskLooKup.data (), &DiskLookup::pending,
//             mPlayListDAO.data (), &IPlayListDAO::beginTransaction);

//    connect (mDiskLooKup.data (),
//             &DiskLookup::finished,
//             [this] {
//        mTagParserManager.data ()->startParserLoop ();
//        emit searchingFinished ();
//    });

//    connect (mDiskLooKup.data (),
//             &DiskLookup::fileFound,
//             [this]
//             (const QString &path, const QString &file, const qint64 &size) {
//        QString hash = Util::calculateHash (path.toLocal8Bit ()
//                                            + file.toLocal8Bit ()
//                                            + QString::number (size));

//        qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;

//        emit searching (path, file, size);

//        PhoenixPlayer::SongMetaData *data = new PhoenixPlayer::SongMetaData(0);
//        data->setMeta (Common::SongMetaTags::E_FileName, file);
//        data->setMeta (Common::SongMetaTags::E_FilePath, path);
//        data->setMeta (Common::SongMetaTags::E_Hash, hash);

//        mTagParserManager.data ()->addItem (data, false);
//    });


    foreach (QString s, mSettings->getMusicDirs ()) {
        mDiskLooKup.data ()->addLookupDir (s, false);
    }
    return mDiskLooKup.data ()->startLookup ();
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
    return mPlayListDAO.data ()->insertPlayList (playListName);
}

bool MusicLibraryManager::deletePlayList(const QString &playListHash)
{
    return mPlayListDAO.data ()->deletePlayList (playListHash);
}

QString MusicLibraryManager::playingSongHash()
{
    if (mCurrentSongHash.isEmpty ()) {
        qDebug()<<"try playingSongHash from settings";
        mCurrentSongHash = mSettings->getLastPlayedSong ();
        if (mCurrentSongHash.isEmpty ()
                && !mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash).isEmpty ()) {
            qDebug()<<"try playingSongHash from first from library";
            mCurrentSongHash = mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash).first ();
        }
    }
    qDebug()<<"playingSongHash is "<<mCurrentSongHash;
    return mCurrentSongHash;
}

void MusicLibraryManager::setPlayingSongHash(const QString &newHash)
{
    if (newHash.isEmpty ())
        return;
    mCurrentSongHash = newHash;
    emit playingSongChanged ();
}

QString MusicLibraryManager::firstSongHash()
{
    mCurrentSongHash = mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash).first ();
    return mCurrentSongHash;
}

QString MusicLibraryManager::lastSongHash()
{
    mCurrentSongHash = mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash).last ();
    return mCurrentSongHash;
}

void MusicLibraryManager::nextSong()
{
    QStringList list = mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash);
    int index = list.indexOf (mCurrentSongHash) +1;
    if (index >= list.size ())
        index = 0;
    mCurrentSongHash = list.at (index);
    emit playingSongChanged ();
}

void MusicLibraryManager::preSong()
{
    QStringList list = mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash);
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
    n = n % mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash).size ();
    mCurrentSongHash = mPlayListDAO.data ()->getSongHashList (mCurrentPlayListHash).at (n);
    emit playingSongChanged ();
}

QStringList MusicLibraryManager::querySongMetaElement(Common::SongMetaTags targetColumn,
                                                      const QString &hash,
                                                      bool skipDuplicates)
{
    QStringList list;
    if (hash.isEmpty ()) {
        list = mPlayListDAO.data ()
                ->queryMusicLibrary (targetColumn,
                                     Common::SongMetaTags::E_FirstFlag, //UnUsed
                                     QString(),
                                     skipDuplicates);
    } else {
        list = mPlayListDAO.data ()
                ->queryMusicLibrary (targetColumn,
                                     Common::SongMetaTags::E_Hash,
                                     hash,
                                     skipDuplicates);
    }
    qDebug()<<" query result "<< list;
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
        list = mPlayListDAO.data ()
                ->queryPlayList (targetColumn,
                                 Common::PlayListFirstFlag,
                                 QString());
    } else {
        list = mPlayListDAO.data ()
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
    return mPlayListDAO.data ()
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
    return mPlayListDAO.data ()
            ->updatePlayList (Common::PlayListSongHashes,
                              playListHash,
                              songHash,
                              false);
}

bool MusicLibraryManager::init()
{
     mCurrentSongHash = mSettings->getLastPlayedSong ();
     mCurrentPlayListHash = mSettings->getPlayListHash ();
//    //本地歌曲扫描线程
//    if (mDiskLooKupThread.isNull ())
//        mDiskLooKupThread = new QThread(this);

//    if (mDiskLooKup.isNull ())
//        mDiskLooKup = new DiskLookup(0);
//    mDiskLooKup.data ()->moveToThread (mDiskLooKupThread);

    if (mPlayListDAO.isNull ())
        mPlayListDAO = mPluginLoader->getCurrentPlayListDAO ();
    if (!mPlayListDAO.data ()->initDataBase ()) {
        qDebug()<<"initDataBase error";
    }

//    //歌曲tag读取线程
//    if (mTagParserThread.isNull ())
//        mTagParserThread = new QThread(this);
//    if (mTagParserManager.isNull ())
//        mTagParserManager = new TagParserManager(0);
//    mTagParserManager.data ()->moveToThread (mTagParserThread);

//    //signal/slot
//    connect (mDiskLooKupThread.data (), &QThread::finished,
//             mDiskLooKup.data (), &DiskLookup::deleteLater);

//    connect (mDiskLooKup.data (), &DiskLookup::pending,
//             mPlayListDAO.data (), &IPlayListDAO::beginTransaction);

//    connect (mDiskLooKup.data (),
//             &DiskLookup::finished,
//             [this] {
//        mTagParserManager.data ()->startParserLoop ();
//        emit searchingFinished ();
//    });

//    connect (mDiskLooKup.data (),
//             &DiskLookup::fileFound,
//             [this]
//             (const QString &path, const QString &file, const qint64 &size) {
//        QString hash = Util::calculateHash (path.toLocal8Bit ()
//                                            + file.toLocal8Bit ()
//                                            + QString::number (size));

//        qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;

//        emit searching (path, file, size);

//        PhoenixPlayer::SongMetaData *data = new PhoenixPlayer::SongMetaData(0);
//        data->setMeta (Common::SongMetaTags::E_FileName, file);
//        data->setMeta (Common::SongMetaTags::E_FilePath, path);
//        data->setMeta (Common::SongMetaTags::E_Hash, hash);

//        mTagParserManager.data ()->addItem (data, false);
//    });

//    connect (mTagParserManager.data (), &TagParserManager::parserPending, [] {
//        qDebug()<<"********************* parserPending";
//    });
//    connect (mTagParserThread.data (), &QThread::finished,
//             mTagParserManager.data (), &TagParserManager::deleteLater);

//    connect (mTagParserManager.data (), &TagParserManager::parserQueueFinished, [this]{
//        //写入数据库
//        qDebug()<<"********************* parserQueueFinished";
//        mPlayListDAO.data ()->commitTransaction ();
//    });

//    //发送一个songhash change的信号,以便于qml界面在初始化后刷新
//    emit playingSongChanged ();
//    emit playListChanged ();

    isInit = true;

    return true;
}

void MusicLibraryManager::initTagParserManager()
{
    //歌曲tag读取线程
    if (mTagParserThread.isNull ())
        mTagParserThread = new QThread(this);
    if (mTagParserManager.isNull ()) {
        mTagParserManager = new TagParserManager(0);
        mTagParserManager.data ()->moveToThread (mTagParserThread);

        connect (mTagParserManager.data (), &TagParserManager::parserPending, [] {
            qDebug()<<"********************* parserPending";
        });
        connect (mTagParserThread.data (), &QThread::finished,
                 mTagParserManager.data (), &TagParserManager::deleteLater);

        connect (mTagParserManager.data (), &TagParserManager::parserQueueFinished, [this]{
            //写入数据库
            qDebug()<<"********************* parserQueueFinished";
            mPlayListDAO.data ()->commitTransaction ();
        });

    }
}

//void MusicLibraryManager::fileFound(QString path, QString file, qint64 size)
//{
//    QString hash = Util::calculateHash (path.toLocal8Bit ()+ file.toLocal8Bit ()+ QString::number (size));
//    qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;

//    emit searching (path, file, size);

//    PhoenixPlayer::SongMetaData data;
//    data.setFileName (file);
//    data.setFilePath (path);
//    data.setFileSize (size);
//    data.setHash (hash);

//    mPlayListDAO->insertMetaData (&data);
//}

} //MusicLibrary
} //PhoenixPlayer

