
#include <QThread>
#include <QDebug>
#include <QPointer>

#include "MusicLibraryManager.h"
//#include "IPlayList.h"
#include "IPlayListDAO.h"
#include "PlayListDAOLoader.h"
#include "DiskLookup.h"
#include "Settings.h"
#include "Util.h"
#include "SongMetaData.h"
#include "Common.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : QObject(parent)
{
    mThread = 0;
    mDiskLooKup = 0;
    mDAOLoader = 0;
    mPlayListDAO = 0;
    mCurrentSongHash = QString();
    mCurrentPlayListHash = QString();

    init();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
    if (!mSettings.isNull ()) {
        mSettings->setLastPlayedSong (mCurrentSongHash);
    }

    if (mDiskLooKup)
        mDiskLooKup->stopLookup ();

    if (mThread) {
        qDebug()<<"wait for thread";
        mThread->quit ();
        mThread->wait (3 * 60 * 1000);
    }
    mThread->deleteLater ();

    qDebug()<<"after delete thread";
    if (mDAOLoader)
        mDAOLoader->deleteLater ();

    qDebug()<<"after delete mDAOLoader";
    if (mPlayListDAO)
        mPlayListDAO->deleteLater ();

    qDebug()<<">>>>>>>> after "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
}

MusicLibraryManager *MusicLibraryManager::getInstance()
{
    static MusicLibraryManager m;
    return &m;
}

void MusicLibraryManager::setSettings(Settings *settings)
{
    mSettings = settings;
    mCurrentSongHash = mSettings->getLastPlayedSong ();
    mCurrentPlayListHash = mSettings->getPlayListHash ();
}

bool MusicLibraryManager::scanLocalMusic()
{
    if (mSettings.isNull ()) {
        qDebug()<<__FUNCTION__<<" can't find settings";
        return false;
    }
    if (mThread.isNull ()) {
        mThread = new QThread(this);
    }
    if (mDiskLooKup.isNull ()) {
        mDiskLooKup = new DiskLookup(0);
        mDiskLooKup.data ()->moveToThread (mThread);
    }
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
    return mPlayListDAO->insertPlayList (playListName);
}

bool MusicLibraryManager::deletePlayList(const QString &playListHash)
{
    return mPlayListDAO->deletePlayList (playListHash);
}

QString MusicLibraryManager::playingSongHash()
{
    if (mCurrentSongHash.isEmpty ()) {
        if (!mSettings.isNull ()) {
            qDebug()<<"try playingSongHash from settings";
            mCurrentSongHash = mSettings.data ()->getLastPlayedSong ();
        }
        if (mCurrentSongHash.isEmpty ()
                && !mPlayListDAO->getSongHashList (mCurrentPlayListHash).isEmpty ()) {
            qDebug()<<"try playingSongHash from first from library";
            mCurrentSongHash = mPlayListDAO->getSongHashList (mCurrentPlayListHash).first ();
        }
    }
    qDebug()<<"playingSongHash is "<<mCurrentSongHash;
    return mCurrentSongHash;
}

QString MusicLibraryManager::firstSongHash()
{
    mCurrentSongHash = mPlayListDAO->getSongHashList (mCurrentPlayListHash).first ();
    return mCurrentSongHash;
}

QString MusicLibraryManager::lastSongHash()
{
    mCurrentSongHash = mPlayListDAO->getSongHashList (mCurrentPlayListHash).last ();
    return mCurrentSongHash;
}

void MusicLibraryManager::nextSong()
{
    QStringList list = mPlayListDAO->getSongHashList (mCurrentPlayListHash);
    int index = list.indexOf (mCurrentSongHash) +1;
    if (index >= list.size ())
        index = 0;
    mCurrentSongHash = list.at (index);
    emit playingSongChanged ();
}

void MusicLibraryManager::preSong()
{
    QStringList list = mPlayListDAO->getSongHashList (mCurrentPlayListHash);
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
    n = n % mPlayListDAO->getSongHashList (mCurrentPlayListHash).size ();
    mCurrentSongHash = mPlayListDAO->getSongHashList (mCurrentPlayListHash).at (n);
    emit playingSongChanged ();
}

QStringList MusicLibraryManager::querySongMetaElement(Common::MusicLibraryElement targetColumn, const QString &hash, bool skipDuplicates)
{
//    qDebug()<<"querySongMetaElement "
//           << targetColumn
//           <<" hash "
//          <<hash
//         << " "
//         <<skipDuplicates;

    QStringList list;
    if (hash.isEmpty ()) {
        list = mPlayListDAO->queryMusicLibrary (targetColumn, Common::E_NULLElement, QString(), skipDuplicates);
    } else {
        list = mPlayListDAO->queryMusicLibrary (targetColumn, Common::E_Hash, hash, skipDuplicates);
    }
    qDebug()<<" query result "<< list;
    return list;
}

QStringList MusicLibraryManager::querySongMetaElementByIndex(int columnIndex, const QString &hash, bool skipDuplicates)
{
    return querySongMetaElement (Common::MusicLibraryElement(columnIndex), hash, skipDuplicates);
}

QStringList MusicLibraryManager::queryPlayListElement(Common::PlayListElement targetColumn, const QString &hash)
{
//    qDebug()<<"queryPlaylist "
//           << targetColumn
//           <<" hash "
//          <<hash;
    QStringList list;
    if (hash.isEmpty ()) {
        list = mPlayListDAO->queryPlayList (targetColumn, Common::E_PlayListNullElement, QString());
    } else {
        list = mPlayListDAO->queryPlayList (targetColumn, Common::E_PlayListHash, hash);
    }

    qDebug()<<" query result "<< list;
    return list;
}

QStringList MusicLibraryManager::queryPlayListElementByIndex(int index, const QString &hash)
{
    return queryPlayListElement (Common::PlayListElement(index), hash);
}

bool MusicLibraryManager::insertToPlayList(const QString &playListHash, const QString &newSongHash)
{
    if (playListHash.isEmpty () || newSongHash.isEmpty ()) {
        qDebug()<<"playListHash or newSongHash is empty";
        return false;
    }
    return mPlayListDAO->updatePlayList (Common::E_PlayListSongHashes, playListHash, newSongHash, true);
}

bool MusicLibraryManager::deleteFromPlayList(const QString &playListHash, const QString &songHash, bool deleteFromStorage)
{
    Q_UNUSED(deleteFromStorage)

    if (playListHash.isEmpty () || songHash.isEmpty ()) {
        qDebug()<<"playListHash or newSongHash is empty";
        return false;
    }
    //TODO: 需要添加从本地删除的功能
    return mPlayListDAO->updatePlayList (Common::E_PlayListSongHashes, playListHash, songHash, false);
}

bool MusicLibraryManager::init()
{
    if (mThread.isNull ())
        mThread = new QThread(this);

    if (mDiskLooKup.isNull ())
        mDiskLooKup = new DiskLookup(0);
    mDiskLooKup.data ()->moveToThread (mThread);

    if (!mDAOLoader)
        mDAOLoader = new PlayListDAOLoader(this);

    if (!mPlayListDAO)
        mPlayListDAO = mDAOLoader->getPlayListDAO ();
    if (!mPlayListDAO->initDataBase ()) {
        qDebug()<<"initDataBase error";
    }

    connect (mThread.data (), &QThread::finished, mDiskLooKup.data (), &DiskLookup::deleteLater);
    connect (mDiskLooKup.data (), &DiskLookup::pending, mPlayListDAO, &IPlayListDAO::beginTransaction);

    connect (mDiskLooKup.data (), &DiskLookup::finished, [this] {
         mPlayListDAO->commitTransaction ();
         emit searchingFinished ();
    });

    connect (mDiskLooKup.data (), &DiskLookup::fileFound,
             [this](const QString &path, const QString &file, const qint64 &size) {
        QString hash = Util::calculateHash (path.toLocal8Bit ()+ file.toLocal8Bit ()+ QString::number (size));

        qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;

        emit searching (path, file, size);

        PhoenixPlayer::SongMetaData data;
        data.setFileName (file);
        data.setFilePath (path);
        data.setFileSize (size);
        data.setHash (hash);

        mPlayListDAO->insertMetaData (&data);
    });

//    //发送一个songhash change的信号,以便于qml界面在初始化后刷新
//    emit playingSongChanged ();
//    emit playListChanged ();
}

void MusicLibraryManager::fileFound(QString path, QString file, qint64 size)
{
    QString hash = Util::calculateHash (path.toLocal8Bit ()+ file.toLocal8Bit ()+ QString::number (size));
    qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;

    emit searching (path, file, size);

    PhoenixPlayer::SongMetaData data;
    data.setFileName (file);
    data.setFilePath (path);
    data.setFileSize (size);
    data.setHash (hash);

    mPlayListDAO->insertMetaData (&data);
}

} //MusicLibrary
} //PhoenixPlayer

