
#include <QThread>
#include <QDebug>

#include "MusicLibraryManager.h"
//#include "IPlayList.h"
#include "IPlayListDAO.h"
#include "PlayListDAOLoader.h"
#include "DiskLookup.h"
#include "Settings.h"
#include "Util.h"
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace PlayList {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : QObject(parent)
{
    mThread = 0;
    mDiskLooKup = 0;
    mDAOLoader = 0;
    mPlayListDAO = 0;
    mCurrentSongHash = QString();

    init();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
    if (mSettings)
        mSettings->setLastPlayedSong (mCurrentSongHash);
    if (mDiskLooKup)
        mDiskLooKup->stopLookup ();

    if (mThread) {
        qDebug()<<"wait for thread";
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
}

bool MusicLibraryManager::scanLocalMusic()
{
    if (!mSettings) {
        qDebug()<<__FUNCTION__<<" can't find settings";
        return false;
    }

    foreach (QString s, mSettings->getMusicDirs ()) {
        mDiskLooKup->addLookupDir (s, false);
    }
    return mDiskLooKup->startLookup ();
}

QString MusicLibraryManager::playingSong()
{
    if (mCurrentSongHash.isEmpty ()) {
        if (mSettings) {
            mCurrentSongHash = mSettings->getLastPlayedSong ();
        }
        if (mCurrentSongHash.isEmpty () && !mPlayListDAO->getSongHashList ().isEmpty ())
            mCurrentSongHash = mPlayListDAO->getSongHashList ().first ();
    }
    return mCurrentSongHash;
}

void MusicLibraryManager::nextSong()
{
    QStringList list = mPlayListDAO->getSongHashList ();
    int index = list.indexOf (mCurrentSongHash) +1;
    if (index >= list.size ())
        index = 0;
    mCurrentSongHash = list.at (index);
    emit playingSongChanged ();
}

void MusicLibraryManager::preSong()
{
    QStringList list = mPlayListDAO->getSongHashList ();
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
    n = n % mPlayListDAO->getSongHashList ().size ();
    mCurrentSongHash = mPlayListDAO->getSongHashList ().at (n);
    emit playingSongChanged ();
}

QStringList MusicLibraryManager::querySongMetaElement(Common::MusicLibraryElement targetColumn, const QString &hash, bool skipDuplicates)
{
    qDebug()<<"querySongMetaElement "
           << targetColumn
           <<" hash "
          <<hash
         << " "
         <<skipDuplicates;

    QStringList list;
    if (hash.isEmpty ()) {
        list = mPlayListDAO->queryColumn (targetColumn, Common::E_NULLElement, QString(), skipDuplicates);
    } else {
        list = mPlayListDAO->queryColumn (targetColumn, Common::E_Hash, hash, skipDuplicates);
    }
    qDebug()<<" query result "<< list;
    return list;
}

bool MusicLibraryManager::init()
{
    if (!mThread)
        mThread = new QThread(this);

    if (!mDiskLooKup)
        mDiskLooKup = new DiskLookup(0);
    mDiskLooKup->moveToThread (mThread);

    if (!mDAOLoader)
        mDAOLoader = new PlayListDAOLoader(this);

    if (!mPlayListDAO)
        mPlayListDAO = mDAOLoader->getPlayListDAO ();
    if (!mPlayListDAO->initDataBase ()) {
        qDebug()<<"initDataBase error";
    }

    connect (mDiskLooKup, SIGNAL(fileFound(QString,QString,qint64)), this, SLOT(fileFound(QString,QString,qint64)));
    connect (mDiskLooKup, &DiskLookup::pending, mPlayListDAO, &IPlayListDAO::beginTransaction);
    connect (mDiskLooKup, SIGNAL(finished()), this, SIGNAL(searchingFinished()));
    connect (mDiskLooKup, &DiskLookup::finished, mPlayListDAO, &IPlayListDAO::commitTransaction);
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












} //PlayList
} //PhoenixPlayer

