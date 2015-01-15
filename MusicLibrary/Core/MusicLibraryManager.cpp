
#include <QThread>
#include <QDebug>

#include "MusicLibraryManager.h"
//#include "IPlayList.h"
#include "IPlayListDAO.h"
#include "PlayListDAOLoader.h"
#include "DiskLookup.h"
#include "Settings.h"
#include "Util.h"

namespace PhoenixPlayer {
namespace PlayList {

MusicLibraryManager::MusicLibraryManager(QObject *parent)
    : QObject(parent)
{
    mThread = 0;
    mDiskLooKup = 0;
    mDAOLoader = 0;
    mPlayListDAO = 0;

    init();
}

MusicLibraryManager::~MusicLibraryManager()
{
    qDebug()<<">>>>>>>> "<< __FUNCTION__ <<" <<<<<<<<<<<<<<<<";
    if (mDiskLooKup)
        mDiskLooKup->stopLookup ();

    if (mThread) {
        qDebug()<<"wait for thread";
        mThread->wait (3 * 60 * 1000);
    }
    mThread->deleteLater ();

    if (mPlayListDAO)
        mPlayListDAO->deleteLater ();
    if (mDAOLoader)
        mDAOLoader->deleteLater ();
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
}

void MusicLibraryManager::fileFound(QString path, QString file, qint64 size)
{
    QString hash = Util::calculateHash (path.toLocal8Bit ()+ file.toLocal8Bit ()+ QString::number (size));
    qDebug()<<"=== find file "<<path<<" "<<file<<" hash "<<hash;
}












} //PlayList
} //PhoenixPlayer

