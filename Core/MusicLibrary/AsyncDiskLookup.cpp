#include "AsyncDiskLookup.h"

#include <QThread>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>

#include "DiskLookup.h"
//#include "PluginLoader.h"
#include "SongMetaData.h"
#include "Util.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
AsyncDiskLookup::AsyncDiskLookup(QObject *parent) :
    QObject(parent)
{
    qDebug()<<Q_FUNC_INFO<<" ====== ";
    mDiskLookupThread = new QThread(0);
    mDiskLookup = new DiskLookup(0);
    mDiskLookup->moveToThread (mDiskLookupThread);


    connect (mDiskLookupThread, &QThread::started, [&] {
        qDebug()<<Q_FUNC_INFO<<" Thread start, we'll start lookup now";
        emit started ();
        mDiskLookup->startLookup ();
    });
    connect (mDiskLookupThread, &QThread::finished, [&] {
        qDebug()<<Q_FUNC_INFO<<" mDiskLookupThread finished";
        if (mDiskLookup->isRunning ())
            mDiskLookup->stopLookup ();
        emit finished (mMetaDataList);
    });
    connect (mDiskLookup, &DiskLookup::finished, [&] {
        qDebug()<<Q_FUNC_INFO<<" DiskLookup finished, we'll try to finish thread";

        if (mDiskLookupThread != nullptr)
            mDiskLookupThread->quit();
    });
    connect (mDiskLookup, &DiskLookup::fileFound,
             this, &AsyncDiskLookup::found,
             Qt::BlockingQueuedConnection);
}

AsyncDiskLookup::~AsyncDiskLookup()
{
    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<";

    if (mDiskLookup->isRunning ())
        mDiskLookup->stopLookup ();
    if (mDiskLookupThread->isRunning ()) {
        mDiskLookupThread->quit ();
        mDiskLookupThread->wait (3 * 60 * 1000);
    }
    mDiskLookup->deleteLater ();
    mDiskLookupThread->deleteLater ();
    if (!mMetaDataList.isEmpty ()) {
        qDeleteAll(mMetaDataList);
        mMetaDataList.clear ();
    }
}

void AsyncDiskLookup::setLookupDirs(const QStringList &dirList, bool lookupImmediately)
{
    if (mDiskLookup->isRunning ()) {
        qWarning()<<Q_FUNC_INFO<<" DiskLookup is running, will ignore this function";
        return;
    }
    foreach (QString s, dirList) {
        mDiskLookup->addLookupDir (s, lookupImmediately);
    }
}

void AsyncDiskLookup::startLookup()
{
    if (mDiskLookup->isRunning ()) {
        qWarning()<<Q_FUNC_INFO<<" DiskLookup is running, will ignore this function";
        return;
    }
    mDiskLookupThread->start (QThread::HighestPriority);
}

void AsyncDiskLookup::found(const QString &path, const QString &file, const qint64 &size)
{
    qDebug()<<Q_FUNC_INFO<<"=== find file "<<path<<" "<<file;
    QString hash = Util::calculateHash (path.toLocal8Bit ()
                                        + file.toLocal8Bit ()
                                        + QString::number (size));

    SongMetaData *data = new SongMetaData(0);
    data->setMeta (Common::SongMetaTags::E_FileName, file);
    data->setMeta (Common::SongMetaTags::E_FilePath, path);
    data->setMeta (Common::SongMetaTags::E_Hash, hash);
    data->setMeta (Common::SongMetaTags::E_FileSize, size);
    mMetaDataList.append (data);
}

} //MusicLibrary
} //PhoenixPlayer
