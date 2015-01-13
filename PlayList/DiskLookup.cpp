#include <QStringList>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>

#include "DiskLookup.h"

namespace PhoenixPlayer {
namespace PlayList {
DiskLookup::DiskLookup(QObject *parent) : QObject(parent)
{
    mLookupLock = false;

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

bool DiskLookup::lookup()
{
    if (mPathList.isEmpty ()) {
        scanDir (QStandardPaths::MusicLocation);
    } else {
        while (!mPathList.isEmpty ()) {
            scanDir (mPathList.takeFirst ());
        }
    }
}

void DiskLookup::setDir(const QString &dirName, bool lookupImmediately)
{
    mPathList.append (dirName);
    if (lookupImmediately) {
        lookup();
    }
}

void DiskLookup::scanDir(const QString &path)
{
    QDir dir(path);
    dir.setFilter (QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList ();
    foreach (QFileInfo info, list) {
        if (info.isDir ()) {
            scanDir (info.absolutePath ());
        } else {
            qDebug()<<" checek file mimetype "<<info.absoluteFilePath ();
            QMimeType type = mQMimeDatabase.mimeTypeForFile (info);
            if (type.inherits ("audio/*")) {
                emit fileFound (path, info.fileName (), info.size ());
            }
        }
    }
}

} //PlayList
} //PhoenixPlayer
