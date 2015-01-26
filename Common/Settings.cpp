#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

#include "Settings.h"

namespace PhoenixPlayer {

const char *KEY_MUSIC_DIR = "MusicDir";
const char *KEY_LAST_SONG = "LastPlayedSongHash";
const char *KEY_PLAY_LIST = "CurrentPlayListHash";
const char *KEY_PLAY_BACKEND = "CurrentPlayBackend";
const char *KEY_MUSIC_IMAGE_CACHE = "MusicImageCache";

Settings::Settings(QObject *parent) : QObject(parent)
{
    mSettings = new QSettings(qApp->organizationName(), qApp->applicationName());

    mDefaultMusicDir = QString("%1/%2").arg (QDir::homePath ())
            .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

    mDefualtMusicImageDir = QString("%1/%2")
            .arg (mDefaultMusicDir)
            .arg (qApp->applicationName());

    QDir dir(mDefualtMusicImageDir);
    if (!dir.exists ()) {
        if (!dir.mkpath (mDefualtMusicImageDir)) {
            qDebug()<<"make music image dir fail";
        }
    }
}

Settings *Settings::getInstance()
{
    static Settings s(0);
    return &s;
}

Settings::~Settings()
{
    mSettings->sync ();
    mSettings->deleteLater ();
}

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
    return mSettings->value (KEY_MUSIC_IMAGE_CACHE, mDefualtMusicImageDir).toString ();
}
} //PhoenixPlayer
