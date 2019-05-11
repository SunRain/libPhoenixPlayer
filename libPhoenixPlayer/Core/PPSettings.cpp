#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QMutex>
#include <QScopedPointer>

#include "PPSettings.h"
#include "AudioMetaObject.h"

namespace PhoenixPlayer {

const char *KEY_MUSIC_DIR = "MusicDir";
const char *KEY_LAST_SONG = "LastPlayedSongHash";
const char *KEY_PLAY_LIST = "CurrentPlayList";
const char *KEY_PLUGIN_PLAY_BACKEND = "Plugin/CurrentPlayBackend";
const char *KEY_PLUGIN_MUSIC_LIBRARY_DAO = "Plugin/CurrentMusicLibraryDAO";
const char *KEY_PLUGIN_OUTPUT = "Plugin/CurrentOutPut";
const char *KEY_PLUGIN_DECODERS = "Plugin/Decoders";
const char *KEY_PLUGIN_METADATA_LOOKUP = "Plugin/MetadataLookup";
const char *KEY_PLUGIN_TAG_PASER = "Plugin/TagPaser";
const char *KEY_MUSIC_IMAGE_CACHE = "MusicImageCache";
const char *KEY_TRACE_LOG = "TraceLog";
const char *KEY_AUTO_FETCH_METADATA = "autoFetchMetaData";
const char *KEY_FETCH_METADATA_MOBILE_NETWORK = "fetchMetaDataMobileNetwork";
const char *KEY_PLAY_LIST_DIR = "PlayListDir";

PPSettings::PPSettings(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings(qApp->organizationName(), qApp->applicationName(),
                              parent);


    QString dataPath = QStandardPaths::writableLocation (QStandardPaths::DataLocation);
#ifdef UBUNTU_TOUCH
    mDefaultMusicDir = QString("%1/Music").arg (dataPath);
#else
    m_defaultMusicDir = QString("%1/%2").arg (QDir::homePath ())
            .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));
#endif
    m_defaultMusicImageDir = QString("%1/Images").arg (dataPath);
//            .arg (QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    m_defaultPlayListDir = QString("%1/PlayList").arg (dataPath);
    m_playListDBPath = m_defaultPlayListDir;

    QDir dir(m_defaultMusicImageDir);
    if (!dir.exists ()) {
        if (!dir.mkpath (m_defaultMusicImageDir)) {
            qDebug()<<Q_FUNC_INFO<<"make music image dir fail";
        }
    }
    dir.setPath(m_defaultMusicDir);
    if (!dir.exists()) {
        if (!dir.mkpath(m_defaultMusicDir))
            qDebug()<<Q_FUNC_INFO<<"make music default dir fail";
    }

    dir.setPath (m_defaultPlayListDir);
    if (!dir.exists ()) {
        if (!dir.mkpath (m_defaultPlayListDir))
            qDebug()<<Q_FUNC_INFO<<"make playlist dir fail";
    }

    m_autoFetchMetadata = false;
    m_fetchMetaDataMobileNetwork = false;
    checkInit ();
}


PPSettings::~PPSettings()
{
    m_settings->sync ();
    m_settings->deleteLater ();
}

QSettings *PPSettings::settings() const
{
    return m_settings;
}

bool PPSettings::setMusicDir(const QStringList &dirList)
{
    if (dirList.isEmpty ())
        return false;

    QStringList list = dirList;
    list.removeDuplicates ();
//    QString last = list.takeLast ();
//    //Only one dir in the list
//    if (list.isEmpty ()) {
//        m_settings->setValue (KEY_MUSIC_DIR, last);
//        return true;
//    }
//    QString tmp;
//    foreach (QString s, list) {
//        tmp += QString("%1||").arg (s);
//    }
//    m_settings->setValue (KEY_MUSIC_DIR, QString("%1%2").arg (tmp).arg (last));
//    m_settings->sync ();
    m_settings->setValue (KEY_MUSIC_DIR, list.join ("||"));
    m_settings->sync ();
    return true;
}

bool PPSettings::addMusicDir(const QString &dir)
{
    if (dir.isEmpty ())
        return false;
    QStringList list = musicDirs ();
    if (list.contains (dir))
        return false;
    list.append (dir);
    return setMusicDir (list);
}


bool PPSettings::deleteMusicDir(const QString &target)
{
    if (target.isEmpty ())
        return false;
    QStringList list = musicDirs ();
    if (list.contains (target))
        list.removeOne (target);
    return setMusicDir (list);

}

QStringList PPSettings::musicDirs()
{
    return m_settings->value (KEY_MUSIC_DIR, QString()).toString ().split ("||");
}


void PPSettings::setLastPlayedSong(const AudioMetaObject &data)
{
//    if (!data)
//        return;
//    QStringList list;
//    list.append ((*data)->path ());
//    list.append ((*data)->name ());
//    list.append (QString::number ((*data)->size ()));
//    m_settings->setValue (KEY_LAST_SONG, list.join ("||"));
//    m_settings->sync ();
    if (data.isEmpty ())
        return;
    m_settings->setValue (KEY_LAST_SONG, data.toJson ());
    m_settings->sync ();
}

AudioMetaObject PPSettings::lastPlayedSong() const
{
//    AudioMetaObject *data = nullptr;
//    QStringList list = m_settings->value (KEY_LAST_SONG, QString()).toString ().split ("||");
//    if (!list.isEmpty () && list.size () == 3)
//        data = new AudioMetaObject(list.at (0), list.at (1), list.at (2).toInt ());
//    return data;
    QByteArray ba = m_settings->value (KEY_LAST_SONG).toByteArray ();
    if (ba.isEmpty () || ba.isNull ())
        return AudioMetaObject();
    return AudioMetaObject::fromJson (ba);
}

bool PPSettings::setCurrentPlayListHash(const QString &hash)
{
    m_settings->setValue (KEY_PLAY_LIST, hash);
    m_settings->sync ();
    return true;
}

QString PPSettings::getPlayListHash()
{
    return m_settings->value (KEY_PLAY_LIST, QString()).toString ();
}

//bool Settings::setPlayBackend(const QString &backendName)
//{
//    m_settings->setValue (KEY_PLAY_BACKEND, backendName);
//    m_settings->sync ();
//    return true;
//}

//QString Settings::getCurrentPlayBackend()
//{
//    return m_settings->value (KEY_PLAY_BACKEND, QString()).toString ();
//}

bool PPSettings::setMusicImageCachePath(const QString &absolutePath)
{
    QDir dir(absolutePath);
    if (!dir.exists ())
        dir.mkpath (absolutePath);

    m_settings->setValue (KEY_MUSIC_IMAGE_CACHE, absolutePath);
    m_settings->sync ();
    return true;
}

QString PPSettings::musicImageCachePath()
{
    return m_settings->value (KEY_MUSIC_IMAGE_CACHE, m_defaultMusicImageDir).toString ();
}

bool PPSettings::setTraceLog(bool trace)
{
    m_settings->setValue(KEY_TRACE_LOG, trace);
    m_settings->sync();
    return true;
}

bool PPSettings::traceLog()
{
    return m_settings->value(KEY_TRACE_LOG, false).toBool();
}

void PPSettings::setConfig(const QString &key, const QString &value)
{
    if (key.isEmpty () || value.isEmpty ())
        return;
    m_settings->setValue (key, value);
    m_settings->sync ();
}

QString PPSettings::getConfig(const QString &key, const QString &defaultValue)
{
    if (key.isEmpty ())
        return defaultValue;
    return m_settings->value (key, defaultValue).toString ();
}

void PPSettings::setConfig(const QString &key, bool value)
{
    if (key.isEmpty ())
        return;
    m_settings->setValue (key, value);
    m_settings->sync ();
}

bool PPSettings::getConfig(const QString &key, bool defaultValue)
{
    if (key.isEmpty ())
        return defaultValue;
    return m_settings->value (key, defaultValue).toBool ();
}

bool PPSettings::autoFetchMetaData()
{
    return m_autoFetchMetadata;
}

void PPSettings::setAutoFetchMetaData(bool autoFetch)
{
    if (m_autoFetchMetadata == autoFetch)
        return;
    m_autoFetchMetadata = autoFetch;
    m_settings->setValue (KEY_AUTO_FETCH_METADATA, autoFetch);
    m_settings->sync ();
    emit autoFetchMetaDataChanged ();
}

bool PPSettings::fetchMetadataOnMobileNetwork()
{
    return m_fetchMetaDataMobileNetwork;
}

void PPSettings::setFetchMetadataOnMobileNetwork(bool fetch)
{
    if (m_fetchMetaDataMobileNetwork == fetch)
        return;
    m_fetchMetaDataMobileNetwork = fetch;
    m_settings->setValue (KEY_FETCH_METADATA_MOBILE_NETWORK, m_fetchMetaDataMobileNetwork);
    m_settings->sync ();
    emit fetchMetaDataMobileNetworkChanged ();
}

QString PPSettings::playListDir() const
{
    return m_settings->value (KEY_PLAY_LIST_DIR, m_defaultPlayListDir).toString ();
}

QString PPSettings::playListDBPath() const
{
    return m_playListDBPath;
}

QString PPSettings::curPlayBackend() const
{
    return m_settings->value (KEY_PLUGIN_PLAY_BACKEND, QString()).toString ();
}

QString PPSettings::curMusicLibraryDAO() const
{
    return m_settings->value (KEY_PLUGIN_MUSIC_LIBRARY_DAO, QString()).toString ();
}

QString PPSettings::curOutPut() const
{
    return m_settings->value (KEY_PLUGIN_OUTPUT, QString()).toString ();
}

void PPSettings::enableDecoder(const QString &libraryFile)
{
    QStringList decoders = decoderLibraries ();
    if (!decoders.contains (libraryFile)) {
        decoders.append (libraryFile);
        m_settings->setValue (KEY_PLUGIN_DECODERS, decoders.join ("||"));
        m_settings->sync ();
    }
}

void PPSettings::disableDecoder(const QString &libraryFile)
{
    QStringList decoders = decoderLibraries ();
    if (decoders.contains (libraryFile)) {
        decoders.removeOne (libraryFile);
        m_settings->setValue (KEY_PLUGIN_DECODERS, decoders.join ("||"));
        m_settings->sync ();
    }
}

QStringList PPSettings::decoderLibraries() const
{
    QString v = m_settings->value (KEY_PLUGIN_DECODERS, QString()).toString ();
    if (v.isEmpty ())
        return QStringList();
    else
        return v.split ("||");
}

void PPSettings::enableMetadataLookup(const QString &libraryFile)
{
    QStringList list = metadataLookupLibraries ();
    if (!list.contains (libraryFile)) {
        list.append (libraryFile);
        m_settings->setValue (KEY_PLUGIN_METADATA_LOOKUP, list.join ("||"));
        m_settings->sync ();
    }
}

void PPSettings::disableMetadataLookup(const QString &libraryFile)
{
    QStringList list = metadataLookupLibraries ();
    if (list.contains (libraryFile)) {
        list.removeOne (libraryFile);
        m_settings->setValue (KEY_PLUGIN_METADATA_LOOKUP, list.join ("||"));
        m_settings->sync ();
    }
}

QStringList PPSettings::metadataLookupLibraries() const
{
    QString v = m_settings->value (KEY_PLUGIN_METADATA_LOOKUP, QString()).toString ();
    if (v.isEmpty ())
        return QStringList();
    else
        return v.split ("||");
}

void PPSettings::enableTagPaser(const QString &libraryFile)
{
    QStringList list = tagPaserLibraries ();
    if (!list.contains (libraryFile)) {
        list.append (libraryFile);
        m_settings->setValue (KEY_PLUGIN_TAG_PASER, list.join ("||"));
        m_settings->sync ();
    }
}

void PPSettings::disableTagPaser(const QString &libraryFile)
{
    QStringList list = tagPaserLibraries ();
    if (list.contains (libraryFile)) {
        list.removeOne (libraryFile);
        m_settings->setValue (KEY_PLUGIN_TAG_PASER, list.join ("||"));
        m_settings->sync ();
    }
}

QStringList PPSettings::tagPaserLibraries() const
{
    QString v = m_settings->value (KEY_PLUGIN_TAG_PASER, QString()).toString ();
    if (v.isEmpty ())
        return QStringList();
    else
        return v.split ("||");
}

void PPSettings::setPlayListDir(QString arg)
{
    if (m_defaultPlayListDir != arg) {
        m_defaultPlayListDir = arg;
        m_settings->setValue (KEY_PLAY_LIST_DIR, m_defaultPlayListDir);
        m_settings->sync ();
        emit playListDirChanged(arg);
    }
}

void PPSettings::setCurPlayBackend(const QString &libraryFile)
{
    if (curPlayBackend () != libraryFile) {
        m_settings->setValue (KEY_PLUGIN_PLAY_BACKEND, libraryFile);
        m_settings->sync ();
        emit curPlayBackendChanged(libraryFile);
    }
}

void PPSettings::setCurMusicLibraryDAO(const QString &libraryFile)
{
    if (curMusicLibraryDAO () != libraryFile) {
        m_settings->setValue (KEY_PLUGIN_MUSIC_LIBRARY_DAO, libraryFile);
        m_settings->sync ();
        emit curMusicLibraryDAOChanged(libraryFile);
    }
}

void PPSettings::setCurOutPut(const QString &libraryFile)
{
    if (curOutPut ()!= libraryFile) {
        m_settings->setValue (KEY_PLUGIN_OUTPUT, libraryFile);
        m_settings->sync ();
        emit curOutPutChanged(libraryFile);
    }
}

void PPSettings::checkInit()
{
    QStringList list = m_settings->allKeys ();
    if (!list.contains (KEY_MUSIC_DIR))
        m_settings->setValue (KEY_MUSIC_DIR, m_defaultMusicDir);
    if (!list.contains (KEY_MUSIC_IMAGE_CACHE))
        m_settings->setValue (KEY_MUSIC_IMAGE_CACHE, m_defaultMusicImageDir);

    m_settings->sync ();
}
} //PhoenixPlayer
