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
//    m_diskLookupThread = new QThread(0);
    m_diskLookup = new DiskLookup(0);
//    m_diskLookup->moveToThread (m_diskLookupThread);


//    connect (m_diskLookupThread, &QThread::started, [&] {
//        qDebug()<<Q_FUNC_INFO<<" Thread start, we'll start lookup now";
//        emit started ();
//        m_diskLookup->startLookup ();
//    });
//    connect (m_diskLookupThread, &QThread::finished, [&] {
//        qDebug()<<Q_FUNC_INFO<<" mDiskLookupThread finished";
//        if (m_diskLookup->isRunning ())
//            m_diskLookup->stopLookup ();
//        emit finished (m_metaDataList);
//    });
    connect (m_diskLookup, &DiskLookup::finished, [&] {
        qDebug()<<Q_FUNC_INFO<<" DiskLookup finished, we'll try to finish thread";
        emit finished (m_metaDataList);
    });
    connect (m_diskLookup, &DiskLookup::fileFound,
             this, &AsyncDiskLookup::found,
             Qt::BlockingQueuedConnection);
}

AsyncDiskLookup::~AsyncDiskLookup()
{
    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<";

    if (m_diskLookup->isRunning ()) {
        m_diskLookup->stopLookup ();
        m_diskLookup->quit ();
        m_diskLookup->wait (3*60*1000);
        m_diskLookup->deleteLater ();
    }

    if (!m_metaDataList.isEmpty ()) {
        qDeleteAll(m_metaDataList);
        m_metaDataList.clear ();
    }
}

void AsyncDiskLookup::setLookupDirs(const QStringList &dirList, bool lookupImmediately)
{
    foreach (QString s, dirList) {
        m_diskLookup->addLookupDir (s, lookupImmediately);
    }
}

void AsyncDiskLookup::startLookup()
{
    m_diskLookup->start (QThread::HighPriority);
}

void AsyncDiskLookup::found(const QString &path, const QString &file, const qint64 &size)
{
    qDebug()<<Q_FUNC_INFO<<QString("find file [%1] in path [%2]").arg (file).arg (path);

    SongMetaData *data = new SongMetaData(path, file, size);
    m_metaDataList.append (data);
}

} //MusicLibrary
} //PhoenixPlayer
