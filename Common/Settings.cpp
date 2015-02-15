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

Settings::Settings(QObject *parent) : QObject(parent)
{
    mSettings = new QSettings(qApp->organizationName(), qApp->applicationName(),
                              parent);

    mDefaultMusicDir = QString("%1/%2").arg (QDir::homePath ())
            .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

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

#ifdef SAILFISH_OS
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
    if (list.isEmpty ()) {
        mSettings->setValue (KEY_MUSIC_DIR, last);
        return true;
    }
    QString tmp;
    foreach (QString s, list) {
        tmp = QString("%1||").arg (s);
    }
    mSettings->setValue (KEY_MUSIC_DIR, QString("%1%2").arg (tmp).arg (last));
    mSettings->sync ();
    return true;
}

QStringList Settings::getMusicDirs()
{
    return mSettings->value (KEY_MUSIC_DIR, mDefaultMusicDir).toStringList ();
}

bool Settings::setLastPlayedSong(const QString &songHash)
{
    mSettings->setValue (KEY_LAST_SONG, songHash);
    mSettings->sync ();
    return true;
}

QString Settings::getLastPlayedSong()
{
    return mSettings->value (KEY_LAST_SONG, QString()).toString ();
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
} //PhoenixPlayer
