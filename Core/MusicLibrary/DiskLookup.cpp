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
DiskLookup::DiskLookup(QObject *parent) : QObject(parent)
{
    qDebug()<<Q_FUNC_INFO;
    m_stopLookupFlag = false;
    m_isRunning = false;
//    mCount = 0;
}

DiskLookup::~DiskLookup()
{
    qDebug()<<Q_FUNC_INFO;
    if (!m_pathList.isEmpty ()) {
        m_pathList.clear ();
    }
}

void DiskLookup::startLookup()
{
    qDebug()<<Q_FUNC_INFO<<"=================== startLookup started";

    emit pending ();

    m_stopLookupFlag = false;
    m_isRunning = true;

    if (m_pathList.isEmpty ()) {
        QString tmp = QString("%1/%2").arg (QDir::homePath ())
                .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

        qDebug()<<"==== Lookup default dir "<<tmp;
        scanDir (tmp);
    } else {
        while (!m_pathList.isEmpty ()) {
            if (m_stopLookupFlag) {
                m_pathList.clear ();
                break;
            }
            scanDir (m_pathList.takeFirst ());
        }
    }
    m_isRunning = false;
    qDebug()<<Q_FUNC_INFO<<"=================== startLookup finish";
    emit finished ();
}

bool DiskLookup::isRunning()
{
    return m_isRunning;
}

void DiskLookup::stopLookup()
{
    m_stopLookupFlag = true;
}

void DiskLookup::addLookupDir(const QString &dirName, bool lookupImmediately)
{
    if (!m_pathList.contains (dirName))
        m_pathList.append (dirName);
    if (lookupImmediately) {
        startLookup();
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
    qDebug()<<Q_FUNC_INFO<<"=== scanDir ["<<path<<"], entryInfoList size "<<list.size ();
    foreach (QFileInfo info, list) {
//        qDebug()<<Q_FUNC_INFO<<" find file ["<<info.absoluteFilePath ()<<"]";
        if (info.isDir ()) {
            qDebug()<<Q_FUNC_INFO<<" current is dir, add to list "<<info.absolutePath ();
            m_pathList.append (info.absolutePath ());
        } else {
            QMimeType type = m_QMimeDatabase.mimeTypeForFile (info);

//            mCount++;
//            qDebug()<<Q_FUNC_INFO<<" find count "<<mCount;

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
