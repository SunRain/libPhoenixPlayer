#include "PPSettingsInternal.h"

#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QMutex>
#include <QScopedPointer>

#include "AudioMetaObject.h"

namespace PhoenixPlayer {

const static char *KEY_MUSIC_DIR = "MusicDir";
const static char *KEY_LAST_SONG = "LastPlayedSongHash";
const static char *KEY_PLAY_LIST = "CurrentPlayList";
const static char *KEY_PLUGIN_PLAY_BACKEND = "Plugin/CurrentPlayBackend";
const static char *KEY_PLUGIN_MUSIC_LIBRARY_DAO = "Plugin/CurrentMusicLibraryDAO";
const static char *KEY_PLUGIN_OUTPUT = "Plugin/CurrentOutPut";
const static char *KEY_PLUGIN_DECODERS = "Plugin/Decoders";
const static char *KEY_PLUGIN_METADATA_LOOKUP = "Plugin/MetadataLookup";
const static char *KEY_PLUGIN_TAG_PARSER = "Plugin/TagPaser";
const static char *KEY_PLUGIN_SPECTTUM_PARSER = "Plugin/SpectrumGenerator";
const static char *KEY_MUSIC_IMAGE_CACHE = "MusicImageCache";
const static char *KEY_TRACE_LOG = "TraceLog";
const static char *KEY_AUTO_FETCH_METADATA = "autoFetchMetaData";
const static char *KEY_FETCH_METADATA_MOBILE_NETWORK = "fetchMetaDataMobileNetwork";
const static char *KEY_PLAY_LIST_DIR = "PlayListDir";


PPSettingsInternal::PPSettingsInternal(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings(qApp->organizationName(), qApp->applicationName(), parent);


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

PPSettingsInternal::~PPSettingsInternal()
{
    qDebug()<<Q_FUNC_INFO<<"------";
    m_settings->sync();
    m_settings->deleteLater();
}

bool PPSettingsInternal::setMusicDir(const QStringList &dirList)
{
    if (dirList.isEmpty ())
        return false;

    QStringList list = dirList;
    list.removeDuplicates ();
    m_settings->setValue (KEY_MUSIC_DIR, list.join ("||"));
    m_settings->sync ();
    return true;
}

bool PPSettingsInternal::addMusicDir(const QString &dir)
{
    if (dir.isEmpty ())
        return false;
    QStringList list = musicDirs ();
    if (list.contains (dir))
        return false;
    list.append (dir);
    return setMusicDir (list);
}


bool PPSettingsInternal::deleteMusicDir(const QString &target)
{
    if (target.isEmpty ())
        return false;
    QStringList list = musicDirs ();
    if (list.contains (target))
        list.removeOne (target);
    return setMusicDir (list);

}

QStringList PPSettingsInternal::musicDirs()
{
    return m_settings->value (KEY_MUSIC_DIR, QString()).toString ().split ("||");
}

void PPSettingsInternal::setLastPlayedSong(const AudioMetaObject &data)
{
    if (data.isHashEmpty())
        return;
    m_settings->setValue (KEY_LAST_SONG, data.toJson ());
    m_settings->sync ();
}

AudioMetaObject PPSettingsInternal::lastPlayedSong() const
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

bool PPSettingsInternal::setCurrentPlayListHash(const QString &hash)
{
    m_settings->setValue (KEY_PLAY_LIST, hash);
    m_settings->sync ();
    return true;
}

QString PPSettingsInternal::getPlayListHash()
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

bool PPSettingsInternal::setMusicImageCachePath(const QString &absolutePath)
{
    QDir dir(absolutePath);
    if (!dir.exists ())
        dir.mkpath (absolutePath);

    m_settings->setValue (KEY_MUSIC_IMAGE_CACHE, absolutePath);
    m_settings->sync ();
    return true;
}

QString PPSettingsInternal::musicImageCachePath()
{
    return m_settings->value (KEY_MUSIC_IMAGE_CACHE, m_defaultMusicImageDir).toString ();
}

void PPSettingsInternal::setTraceLog(bool trace)
{
    m_settings->setValue(KEY_TRACE_LOG, trace);
    m_settings->sync();
}

bool PPSettingsInternal::traceLog()
{
    return m_settings->value(KEY_TRACE_LOG, false).toBool();
}

void PPSettingsInternal::setConfig(const QString &key, const QString &value)
{
    if (key.isEmpty () || value.isEmpty ())
        return;
    m_settings->setValue (key, value);
    m_settings->sync ();
}

QString PPSettingsInternal::getConfig(const QString &key, const QString &defaultValue)
{
    if (key.isEmpty ())
        return defaultValue;
    return m_settings->value (key, defaultValue).toString ();
}

void PPSettingsInternal::setConfig(const QString &key, bool value)
{
    if (key.isEmpty ())
        return;
    m_settings->setValue (key, value);
    m_settings->sync ();
}

bool PPSettingsInternal::getConfig(const QString &key, bool defaultValue)
{
    if (key.isEmpty ())
        return defaultValue;
    return m_settings->value (key, defaultValue).toBool ();
}



void PPSettingsInternal::setAutoFetchMetaData(bool autoFetch)
{
    if (m_autoFetchMetadata == autoFetch)
        return;
    m_autoFetchMetadata = autoFetch;
    m_settings->setValue (KEY_AUTO_FETCH_METADATA, autoFetch);
    m_settings->sync ();
    emit autoFetchMetaDataChanged ();
}



void PPSettingsInternal::setFetchMetadataOnMobileNetwork(bool fetch)
{
    if (m_fetchMetaDataMobileNetwork == fetch)
        return;
    m_fetchMetaDataMobileNetwork = fetch;
    m_settings->setValue (KEY_FETCH_METADATA_MOBILE_NETWORK, m_fetchMetaDataMobileNetwork);
    m_settings->sync ();
    emit fetchMetaDataMobileNetworkChanged ();
}

QString PPSettingsInternal::playListDir() const
{
    return m_settings->value (KEY_PLAY_LIST_DIR, m_defaultPlayListDir).toString ();
}



QString PPSettingsInternal::curPlayBackend() const
{
    return m_settings->value (KEY_PLUGIN_PLAY_BACKEND, QString()).toString ();
}

QString PPSettingsInternal::curMusicLibraryDAO() const
{
    return m_settings->value (KEY_PLUGIN_MUSIC_LIBRARY_DAO, QString()).toString ();
}

QString PPSettingsInternal::curOutPut() const
{
    return m_settings->value (KEY_PLUGIN_OUTPUT, QString()).toString ();
}

void PPSettingsInternal::enableDecoder(const QString &libraryFile)
{
    QStringList decoders = decoderLibraries ();
    if (!decoders.contains (libraryFile)) {
        decoders.append (libraryFile);
        m_settings->setValue (KEY_PLUGIN_DECODERS, decoders.join ("||"));
        m_settings->sync ();
    }
}

void PPSettingsInternal::disableDecoder(const QString &libraryFile)
{
    QStringList decoders = decoderLibraries ();
    if (decoders.contains (libraryFile)) {
        decoders.removeOne (libraryFile);
        m_settings->setValue (KEY_PLUGIN_DECODERS, decoders.join ("||"));
        m_settings->sync ();
    }
}

QStringList PPSettingsInternal::decoderLibraries() const
{
    QString v = m_settings->value (KEY_PLUGIN_DECODERS, QString()).toString ();
    if (v.isEmpty ())
        return QStringList();
    else
        return v.split ("||");
}

void PPSettingsInternal::enableMetadataLookup(const QString &libraryFile)
{
    QStringList list = metadataLookupLibraries ();
    if (!list.contains (libraryFile)) {
        list.append (libraryFile);
        m_settings->setValue (KEY_PLUGIN_METADATA_LOOKUP, list.join ("||"));
        m_settings->sync ();
    }
}

void PPSettingsInternal::disableMetadataLookup(const QString &libraryFile)
{
    QStringList list = metadataLookupLibraries ();
    if (list.contains (libraryFile)) {
        list.removeOne (libraryFile);
        m_settings->setValue (KEY_PLUGIN_METADATA_LOOKUP, list.join ("||"));
        m_settings->sync ();
    }
}

QStringList PPSettingsInternal::metadataLookupLibraries() const
{
    QString v = m_settings->value (KEY_PLUGIN_METADATA_LOOKUP, QString()).toString ();
    if (v.isEmpty ())
        return QStringList();
    else
        return v.split ("||");
}

void PPSettingsInternal::enableTagParser(const QString &libraryFile)
{
    QStringList list = tagParserLibraries ();
    if (!list.contains (libraryFile)) {
        list.append (libraryFile);
        m_settings->setValue (KEY_PLUGIN_TAG_PARSER, list.join ("||"));
        m_settings->sync ();
    }
}

void PPSettingsInternal::disableTagParser(const QString &libraryFile)
{
    QStringList list = tagParserLibraries ();
    if (list.contains (libraryFile)) {
        list.removeOne (libraryFile);
        m_settings->setValue (KEY_PLUGIN_TAG_PARSER, list.join ("||"));
        m_settings->sync ();
    }
}

QStringList PPSettingsInternal::tagParserLibraries() const
{
    QString v = m_settings->value (KEY_PLUGIN_TAG_PARSER, QString()).toString ();
    if (v.isEmpty ())
        return QStringList();
    else
        return v.split ("||");
}

void PPSettingsInternal::enableSpectrumGenerator(const QString &lib)
{
    QStringList list = spectrumGeneratorLibraries();
    if (!list.contains(lib)) {
        list.append(lib);
        m_settings->setValue(KEY_PLUGIN_SPECTTUM_PARSER, list.join("||"));
        m_settings->sync();
    }
}

void PPSettingsInternal::disableSpectrumGenerator(const QString &lib)
{
    QStringList list = spectrumGeneratorLibraries();
    if (list.contains(lib)) {
        list.removeOne(lib);
        m_settings->setValue(KEY_PLUGIN_SPECTTUM_PARSER, list.join("||"));
        m_settings->sync();
    }
}

QStringList PPSettingsInternal::spectrumGeneratorLibraries() const
{
    QString str = m_settings->value(KEY_PLUGIN_SPECTTUM_PARSER, QString()).toString();
    if (str.isEmpty()) {
        return QStringList();
    }
    return str.split("||");
}

void PPSettingsInternal::setPlayListDir(QString arg)
{
    if (m_defaultPlayListDir != arg) {
        m_defaultPlayListDir = arg;
        m_settings->setValue (KEY_PLAY_LIST_DIR, m_defaultPlayListDir);
        m_settings->sync ();
        emit playListDirChanged(arg);
    }
}

void PPSettingsInternal::setCurPlayBackend(const QString &libraryFile)
{
    if (curPlayBackend () != libraryFile) {
        m_settings->setValue (KEY_PLUGIN_PLAY_BACKEND, libraryFile);
        m_settings->sync ();
        emit curPlayBackendChanged(libraryFile);
    }
}

void PPSettingsInternal::setCurMusicLibraryDAO(const QString &libraryFile)
{
    if (curMusicLibraryDAO () != libraryFile) {
        m_settings->setValue (KEY_PLUGIN_MUSIC_LIBRARY_DAO, libraryFile);
        m_settings->sync ();
        emit curMusicLibraryDAOChanged(libraryFile);
    }
}

void PPSettingsInternal::setCurOutPut(const QString &libraryFile)
{
    if (curOutPut ()!= libraryFile) {
        m_settings->setValue (KEY_PLUGIN_OUTPUT, libraryFile);
        m_settings->sync ();
        emit curOutPutChanged(libraryFile);
    }
}

void PPSettingsInternal::checkInit()
{
    QStringList list = m_settings->allKeys ();
    if (!list.contains (KEY_MUSIC_DIR))
        m_settings->setValue (KEY_MUSIC_DIR, m_defaultMusicDir);
    if (!list.contains (KEY_MUSIC_IMAGE_CACHE))
        m_settings->setValue (KEY_MUSIC_IMAGE_CACHE, m_defaultMusicImageDir);

    m_settings->sync ();
}
} //PhoenixPlayer
