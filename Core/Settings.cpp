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
const char *KEY_FETCH_METADATA_MOBILE_NETWORK = "fetchMetaDataMobileNetwork";

Settings::Settings(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings(qApp->organizationName(), qApp->applicationName(),
                              parent);

#ifdef UBUNTU_TOUCH
    QString dataPath = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
//    QDir d(dataPath);
//    d.cdUp ();
//    dataPath = d.absolutePath ();
    mDefaultMusicDir = QString("%1/Music").arg (dataPath);
#else
    m_defaultMusicDir = QString("%1/%2").arg (QDir::homePath ())
            .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));
#endif
    m_defaultMusicImageDir = QString("%1/Images")
            .arg (QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    QDir dir(m_defaultMusicImageDir);
    if (!dir.exists ()) {
        if (!dir.mkpath (m_defaultMusicImageDir)) {
            qDebug()<<"make music image dir fail";
        }
    }
    dir.setPath(m_defaultMusicDir);
    if (!dir.exists()) {
        if (!dir.mkpath(m_defaultMusicDir))
            qDebug()<<"make music default dir fail";
    }

    m_autoFetchMetadata = false;
    m_fetchMetaDataMobileNetwork = false;
    checkInit ();
}

//Settings *Settings::getInstance()
//{
//    static Settings s(0);
//    return &s;
//}

Settings::~Settings()
{
    m_settings->sync ();
    m_settings->deleteLater ();
}

//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//Settings *Settings::instance()
//{
//    static QMutex mutex;
//    static QScopedPointer<Settings> scp;

//    if (Q_UNLIKELY(scp.isNull())) {
//        qDebug()<<Q_FUNC_INFO<<">>>>>>>> statr new";
//        mutex.lock();
//        scp.reset(new Settings(0));
//        mutex.unlock();
//    }
//    return scp.data();
//}
//#endif

bool Settings::setMusicDir(const QStringList &dirList)
{
    if (dirList.isEmpty ())
        return false;

    QStringList list = dirList;
    list.removeDuplicates ();
    QString last = list.takeLast ();
    //Only one dir in the list
    if (list.isEmpty ()) {
        m_settings->setValue (KEY_MUSIC_DIR, last);
        return true;
    }
    QString tmp;
    foreach (QString s, list) {
        tmp += QString("%1||").arg (s);
    }
    m_settings->setValue (KEY_MUSIC_DIR, QString("%1%2").arg (tmp).arg (last));
    m_settings->sync ();
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
    return m_settings->value (KEY_MUSIC_DIR, m_defaultMusicDir).toString ().split ("||");
}

QString Settings::getLastPlayedSong()
{
    return m_settings->value (KEY_LAST_SONG, QString()).toString ();
}

bool Settings::setLastPlayedSong(const QString &songHash)
{
    m_settings->setValue (KEY_LAST_SONG, songHash);
    m_settings->sync ();
    return true;
}

bool Settings::setCurrentPlayListHash(const QString &hash)
{
    m_settings->setValue (KEY_PLAY_LIST, hash);
    m_settings->sync ();
    return true;
}

QString Settings::getPlayListHash()
{
    return m_settings->value (KEY_PLAY_LIST, QString()).toString ();
}

bool Settings::setPlayBackend(const QString &backendName)
{
    m_settings->setValue (KEY_PLAY_BACKEND, backendName);
    m_settings->sync ();
    return true;
}

QString Settings::getCurrentPlayBackend()
{
    return m_settings->value (KEY_PLAY_BACKEND, QString()).toString ();
}

bool Settings::setMusicImageCachePath(const QString &absolutePath)
{
    QDir dir(absolutePath);
    if (!dir.exists ())
        dir.mkpath (absolutePath);

    m_settings->setValue (KEY_MUSIC_IMAGE_CACHE, absolutePath);
    m_settings->sync ();
    return true;
}

QString Settings::getMusicImageCachePath()
{
    return m_settings->value (KEY_MUSIC_IMAGE_CACHE, m_defaultMusicImageDir).toString ();
}

bool Settings::setTraceLog(bool trace)
{
    m_settings->setValue(KEY_TRACE_LOG, trace);
    m_settings->sync();
    return true;
}

bool Settings::traceLog()
{
    return m_settings->value(KEY_TRACE_LOG, false).toBool();
}

void Settings::setConfig(const QString &key, const QString &value)
{
    if (key.isEmpty () || value.isEmpty ())
        return;
    m_settings->setValue (key, value);
    m_settings->sync ();
}

QString Settings::getConfig(const QString &key, const QString &defaultValue)
{
    if (key.isEmpty ())
        return defaultValue;
    return m_settings->value (key, defaultValue).toString ();
}

void Settings::setConfig(const QString &key, bool value)
{
    if (key.isEmpty ())
        return;
    m_settings->setValue (key, value);
    m_settings->sync ();
}

bool Settings::getConfig(const QString &key, bool defaultValue)
{
    if (key.isEmpty ())
        return defaultValue;
    return m_settings->value (key, defaultValue).toBool ();
}

bool Settings::autoFetchMetaData()
{
    return m_autoFetchMetadata;
}

void Settings::setAutoFetchMetaData(bool autoFetch)
{
    if (m_autoFetchMetadata == autoFetch)
        return;
    m_autoFetchMetadata = autoFetch;
    m_settings->setValue (KEY_AUTO_FETCH_METADATA, autoFetch);
    m_settings->sync ();
    emit autoFetchMetaDataChanged ();
}

bool Settings::fetchMetaDataMobileNetwork()
{
    return m_fetchMetaDataMobileNetwork;
}

void Settings::setFetchMetaDataMobileNetwork(bool fetch)
{
    if (m_fetchMetaDataMobileNetwork == fetch)
        return;
    m_fetchMetaDataMobileNetwork = fetch;
    m_settings->setValue (KEY_FETCH_METADATA_MOBILE_NETWORK, m_fetchMetaDataMobileNetwork);
    m_settings->sync ();
    emit fetchMetaDataMobileNetworkChanged ();
}

void Settings::checkInit()
{
    QStringList list = m_settings->allKeys ();
    if (!list.contains (KEY_MUSIC_DIR))
        m_settings->setValue (KEY_MUSIC_DIR, m_defaultMusicDir);
    if (!list.contains (KEY_MUSIC_IMAGE_CACHE))
        m_settings->setValue (KEY_MUSIC_IMAGE_CACHE, m_defaultMusicImageDir);

    m_settings->sync ();
}
} //PhoenixPlayer
