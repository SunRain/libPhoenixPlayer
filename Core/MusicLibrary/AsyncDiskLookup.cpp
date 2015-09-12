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
    m_diskLookupThread = new QThread(0);
    m_diskLookup = new DiskLookup(0);
    m_diskLookup->moveToThread (m_diskLookupThread);


    connect (m_diskLookupThread, &QThread::started, [&] {
        qDebug()<<Q_FUNC_INFO<<" Thread start, we'll start lookup now";
        emit started ();
        m_diskLookup->startLookup ();
    });
    connect (m_diskLookupThread, &QThread::finished, [&] {
        qDebug()<<Q_FUNC_INFO<<" mDiskLookupThread finished";
        if (m_diskLookup->isRunning ())
            m_diskLookup->stopLookup ();
        emit finished (m_metaDataList);
    });
    connect (m_diskLookup, &DiskLookup::finished, [&] {
        qDebug()<<Q_FUNC_INFO<<" DiskLookup finished, we'll try to finish thread";

        if (m_diskLookupThread)
            m_diskLookupThread->quit();
    });
    connect (m_diskLookup, &DiskLookup::fileFound,
             this, &AsyncDiskLookup::found,
             Qt::BlockingQueuedConnection);
}

AsyncDiskLookup::~AsyncDiskLookup()
{
    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<";

    if (m_diskLookup->isRunning ())
        m_diskLookup->stopLookup ();
    if (m_diskLookupThread->isRunning ()) {
        m_diskLookupThread->quit ();
        m_diskLookupThread->wait (3 * 60 * 1000);
    }
    m_diskLookup->deleteLater ();
    m_diskLookupThread->deleteLater ();
    if (!m_metaDataList.isEmpty ()) {
        qDeleteAll(m_metaDataList);
        m_metaDataList.clear ();
    }
}

void AsyncDiskLookup::setLookupDirs(const QStringList &dirList, bool lookupImmediately)
{
    if (m_diskLookup->isRunning ()) {
        qWarning()<<Q_FUNC_INFO<<" DiskLookup is running, will ignore this function";
        return;
    }
    foreach (QString s, dirList) {
        m_diskLookup->addLookupDir (s, lookupImmediately);
    }
}

void AsyncDiskLookup::startLookup()
{
    if (m_diskLookup->isRunning ()) {
        qWarning()<<Q_FUNC_INFO<<" DiskLookup is running, will ignore this function";
        return;
    }
    m_diskLookupThread->start (QThread::HighestPriority);
}

void AsyncDiskLookup::found(const QString &path, const QString &file, const qint64 &size)
{
    qDebug()<<Q_FUNC_INFO<<QString("find file [%1] in path [%2]").arg (file).arg (path);
    QString hash = Util::calculateHash (path.toLocal8Bit ()
                                        + file.toLocal8Bit ()
                                        + QString::number (size));

    SongMetaData *data = new SongMetaData(0);
    data->setMeta (Common::SongMetaTags::E_FileName, file);
    data->setMeta (Common::SongMetaTags::E_FilePath, path);
    data->setMeta (Common::SongMetaTags::E_Hash, hash);
    data->setMeta (Common::SongMetaTags::E_FileSize, size);
    m_metaDataList.append (data);
}

} //MusicLibrary
} //PhoenixPlayer
