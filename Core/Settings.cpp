#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QMutex>
#include <QScopedPointer>

#include "Settings.h"

namespace PhoenixPlayer {

const char *KEY_MUSIC_DIR = "MusicDir";
const char *KEY_LAST_SONG = "LastPlayedSongHash";
const char *KEY_PLAY_LIST = "CurrentPlayListHash";
const char *KEY_PLAY_BACKEND = "CurrentPlayBackend";
const char *KEY_MUSIC_IMAGE_CACHE = "MusicImageCache";
const char *KEY_TRACE_LOG = "TraceLog";
const char *KEY_AUTO_FETCH_METADATA = "autoFetchMetaData";

Settings::Settings(QObject *parent) : QObject(parent)
{
    mSettings = new QSettings(qApp->organizationName(), qApp->applicationName(),
                              parent);

#ifdef UBUNTU_TOUCH
    QString dataPath = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
//    QDir d(dataPath);
//    d.cdUp ();
//    dataPath = d.absolutePath ();
    mDefaultMusicDir = QString("%1/Music").arg (dataPath);
#else
    mDefaultMusicDir = QString("%1/%2").arg (QDir::homePath ())
            .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));
#endif
    mDefaultMusicImageDir = QString("%1/Images")
            .arg (QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    QDir dir(mDefaultMusicImageDir);
    if (!dir.exists ()) {
        if (!dir.mkpath (mDefaultMusicImageDir)) {
            qDebug()<<"make music image dir fail";
        }
    }
    dir.setPath(mDefaultMusicDir);
    if (!dir.exists()) {
        if (!dir.mkpath(mDefaultMusicDir))
            qDebug()<<"make music default dir fail";
    }

    mAutoFetchMetadata = false;
    checkInit ();
}

//Settings *Settings::getInstance()
//{
//    static Settings s(0);
//    return &s;
//}

Settings::~Settings()
{
    mSettings->sync ();
    mSettings->deleteLater ();
}

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
Settings *Settings::instance()
{
    qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
    static QMutex mutex;
    static QScopedPointer<Settings> scp;

    if (Q_UNLIKELY(scp.isNull())) {
        qDebug()<<">>>>>>>> statr new";
        mutex.lock();
        scp.reset(new Settings(0));
        mutex.unlock();
    }
    qDebug()<<">>>>>>>> return "<<scp.data()->metaObject()->className();;
    return scp.data();
}
#endif

bool Settings::setMusicDir(const QStringList &dirList)
{
    if (dirList.isEmpty ())
        return false;

    QStringList list = dirList;
    list.removeDuplicates ();
    QString last = list.takeLast ();
    //Only one dir in the list
    if (list.isEmpty ()) {
        mSettings->setValue (KEY_MUSIC_DIR, last);
        return true;
    }
    QString tmp;
    foreach (QString s, list) {
        tmp += QString("%1||").arg (s);
    }
    mSettings->setValue (KEY_MUSIC_DIR, QString("%1%2").arg (tmp).arg (last));
    mSettings->sync ();
    return true;
}

bool Settings::addMusicDir(const QString &dir)
{
    if (dir.isEmpty ())
        return false;
    QStringList list = getMusicDirs ();
    if (list.contains (dir))
        return false;
    list.append (dir);
    return setMusicDir (list);
}


bool Settings::deleteMusicDir(const QString &target)
{
    if (target.isEmpty ())
        return false;
    QStringList list = getMusicDirs ();
    if (list.contains (target))
        list.removeOne (target);
    return setMusicDir (list);

}

QStringList Settings::getMusicDirs()
{
    return mSettings->value (KEY_MUSIC_DIR, mDefaultMusicDir).toString ().split ("||");
}

QString Settings::getLastPlayedSong()
{
    return mSettings->value (KEY_LAST_SONG, QString()).toString ();
}

bool Settings::setLastPlayedSong(const QString &songHash)
{
    mSettings->setValue (KEY_LAST_SONG, songHash);
    mSettings->sync ();
}

bool Settings::setCurrentPlayListHash(const QString &hash)
{
    mSettings->setValue (KEY_PLAY_LIST, hash);
    mSettings->sync ();
    return true;
}

QString Settings::getPlayListHash()
{
    return mSettings->value (KEY_PLAY_LIST, QString()).toString ();
}

bool Settings::setPlayBackend(const QString &backendName)
{
    mSettings->setValue (KEY_PLAY_BACKEND, backendName);
    mSettings->sync ();
    return true;
}

QString Settings::getCurrentPlayBackend()
{
    return mSettings->value (KEY_PLAY_BACKEND, QString()).toString ();
}

bool Settings::setMusicImageCachePath(const QString &absolutePath)
{
    QDir dir(absolutePath);
    if (!dir.exists ())
        dir.mkpath (absolutePath);

    mSettings->setValue (KEY_MUSIC_IMAGE_CACHE, absolutePath);
    mSettings->sync ();
    return true;
}

QString Settings::getMusicImageCachePath()
{
    return mSettings->value (KEY_MUSIC_IMAGE_CACHE, mDefaultMusicImageDir).toString ();
}

bool Settings::setTraceLog(bool trace)
{
    mSettings->setValue(KEY_TRACE_LOG, trace);
    mSettings->sync();
    return true;
}

bool Settings::traceLog()
{
    return mSettings->value(KEY_TRACE_LOG, false).toBool();
}

bool Settings::setConfig(const QString &key, const QString &value)
{
    if (key.isEmpty () || value.isEmpty ())
        return false;
    mSettings->setValue (key, value);
    mSettings->sync ();
    return true;
}

QString Settings::getConfig(const QString &key, const QString &defaultValue)
{
    return mSettings->value (key, defaultValue).toString ();
}

bool Settings::autoFetchMetaData()
{
    return mAutoFetchMetadata;
}

void Settings::setAutoFetchMetaData(bool autoFetch)
{
    if (mAutoFetchMetadata == autoFetch)
        return;
    mAutoFetchMetadata = autoFetch;
    mSettings->setValue (KEY_AUTO_FETCH_METADATA, autoFetch);
    mSettings->sync ();
    emit autoFetchMetaDataChanged ();
}

void Settings::checkInit()
{
    QStringList list = mSettings->allKeys ();
    if (!list.contains (KEY_MUSIC_DIR))
        mSettings->setValue (KEY_MUSIC_DIR, mDefaultMusicDir);
    if (!list.contains (KEY_MUSIC_IMAGE_CACHE))
        mSettings->setValue (KEY_MUSIC_IMAGE_CACHE, mDefaultMusicImageDir);

    mSettings->sync ();
}
} //PhoenixPlayer
