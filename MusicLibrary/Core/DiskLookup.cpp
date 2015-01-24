#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>

#include "DiskLookup.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
DiskLookup::DiskLookup(QObject *parent) : QObject(parent)
{
//    mLookupLock = false;
    mStopLookupFlag = false;
}

DiskLookup *DiskLookup::getInstance()
{
    static DiskLookup lookup;
    return &lookup;
}

DiskLookup::~DiskLookup()
{
    if (!mPathList.isEmpty ()) {
        mPathList.clear ();
    }
}

bool DiskLookup::startLookup()
{
    emit pending ();

    mStopLookupFlag = false;

    if (mPathList.isEmpty ()) {
        QString tmp = QString("%1/%2").arg (QDir::homePath ())
                .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

        qDebug()<<"==== Lookup default dir "<<tmp;
        scanDir (tmp);
    } else {
        while (!mPathList.isEmpty ()) {
            if (mStopLookupFlag) {
                mPathList.clear ();
                break;
            }
            scanDir (mPathList.takeFirst ());
        }
    }
    emit finished ();
    return true;
}

bool DiskLookup::stopLookup()
{
    mStopLookupFlag = true;
}

void DiskLookup::addLookupDir(const QString &dirName, bool lookupImmediately)
{
    mPathList.append (dirName);
    if (lookupImmediately) {
        startLookup();
    }
}

void DiskLookup::scanDir(const QString &path)
{
    if (mStopLookupFlag)
        return;

    QDir dir(path);
    if (!dir.exists ())
        return;

    qDebug()<<"=== scanDir "<<path;
    dir.setFilter (QDir::Dirs | QDir::Files | /*QDir::NoSymLinks |*/ QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList ();
    foreach (QFileInfo info, list) {
        if (info.isDir ()) {
            scanDir (info.filePath ());
        } else {
            QMimeType type = mQMimeDatabase.mimeTypeForFile (info);

            //TODO 虽然建议使用inherits方法来检测,但是此处我们需要所有音频文件,
            //所以直接检测mimetype 生成的字符串

//            if (type.inherits ("audio/mpeg")) {
            if (type.name ().contains ("audio") || type.name ().contains ("Audio")) {
//                qDebug()<<" file match "<< info.fileName();
                emit fileFound (path, info.fileName (), info.size ());
            }
        }
    }
}

} //MusicLibrary
} //PhoenixPlayer
