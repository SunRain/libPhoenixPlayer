#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>
#include <QFileSystemWatcher>

#include "DiskLookup.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
DiskLookup::DiskLookup(QObject *parent)
    : QThread(parent)
{
    m_stopLookupFlag = false;
//    m_isRunning = false;
}

DiskLookup::~DiskLookup()
{
    if (!m_pathList.isEmpty ()) {
        m_pathList.clear ();
    }
}

//void DiskLookup::startLookup()
//{
//    qDebug()<<Q_FUNC_INFO<<"=================== startLookup started";

//    emit pending ();

//    m_stopLookupFlag = false;
//    m_isRunning = true;

//    if (m_pathList.isEmpty ()) {
//        QString tmp = QString("%1/%2").arg (QDir::homePath ())
//                .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

//        qDebug()<<"Lookup default dir "<<tmp;
//        scanDir (tmp);
//    } else {
//        while (!m_pathList.isEmpty ()) {
//            m_mutex.lock ();
//            if (m_stopLookupFlag) {
//                m_pathList.clear ();
//                m_mutex.unlock ();
//                break;
//            }
//            QString p = m_pathList.takeFirst ();
//            m_mutex.unlock ();
//            scanDir (p);
//        }
//    }
//    m_isRunning = false;
//    qDebug()<<Q_FUNC_INFO<<"=================== startLookup finish";
//    emit finished ();
//}

//bool DiskLookup::isRunning()
//{
//    return m_isRunning;
//}

void DiskLookup::stopLookup()
{
    m_stopLookupFlag = true;
}

void DiskLookup::addLookupDir(const QString &dirName, bool lookupImmediately)
{
    m_mutex.lock ();
    if (!m_pathList.contains (dirName))
        m_pathList.append (dirName);
    m_mutex.unlock ();
    if (lookupImmediately && !this->isRunning ()) {
        this->start ();
    }
}

void DiskLookup::run()
{
    if (m_pathList.isEmpty ()) {
        QString tmp = QString("%1/%2").arg (QDir::homePath ())
                .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

        qDebug()<<"Lookup default dir "<<tmp;
        m_pathList.append (tmp);
    }
    while (!m_pathList.isEmpty ()) {
        m_mutex.lock ();
        if (m_stopLookupFlag) {
            m_pathList.clear ();
            m_mutex.unlock ();
            break;
        }
        QString p = m_pathList.takeFirst ();
        m_mutex.unlock ();
        scanDir (p);
    }
}

void DiskLookup::scanDir(const QString &path)
{
    if (m_stopLookupFlag)
        return;

    QDir dir(path);
    if (!dir.exists ())
        return;

    dir.setFilter (QDir::Dirs | QDir::Files | /*QDir::NoSymLinks |*/ QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList ();
    qDebug()<<Q_FUNC_INFO<<QString("scanDir [%1], entryInfoList size = %2").arg (path).arg (list.size ());
    foreach (QFileInfo info, list) {
        if (info.isDir ()) {
            m_mutex.lock ();
            qDebug()<<Q_FUNC_INFO<<QString("current item [%1] is dir, add [%2] to list")
                      .arg (info.baseName ()).arg (info.absoluteFilePath ());
            m_pathList.append (info.absoluteFilePath ());
            m_mutex.unlock ();
        } else {
            QMimeType type = m_QMimeDatabase.mimeTypeForFile (info);

            //TODO 虽然建议使用inherits方法来检测,但是此处我们需要所有音频文件,
            //所以直接检测mimetype 生成的字符串

//            if (type.inherits ("audio/mpeg")) {
            if (type.name ().contains ("audio") || type.name ().contains ("Audio")) {
                emit fileFound (path, info.fileName (), info.size ());
            }
        }
    }
}

} //MusicLibrary
} //PhoenixPlayer
