#include "PPSettings.h"

#include <QSettings>
//#include <QDir>
//#include <QStandardPaths>
//#include <QCoreApplication>
//#include <QStringList>
//#include <QDebug>
//#include <QMutex>
//#include <QScopedPointer>

#include "AudioMetaObject.h"
#include "private/SingletonObjectFactory.h"
#include "private/PPSettingsInternal.h"

namespace PhoenixPlayer {


PPSettings::PPSettings(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->settingsInternal();

    connect(m_internal.data(), &PPSettingsInternal::autoFetchMetaDataChanged,
            this, &PPSettings::autoFetchMetaDataChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &PPSettingsInternal::fetchMetaDataMobileNetworkChanged,
            this, &PPSettings::fetchMetaDataMobileNetworkChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &PPSettingsInternal::playListDirChanged,
            this, &PPSettings::playListDirChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &PPSettingsInternal::curPlayBackendChanged,
            this, &PPSettings::curPlayBackendChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &PPSettingsInternal::curMusicLibraryDAOChanged,
            this, &PPSettings::curMusicLibraryDAOChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &PPSettingsInternal::curOutPutChanged,
            this, &PPSettings::curOutPutChanged, Qt::QueuedConnection);

}

PPSettings::~PPSettings()
{
    m_internal->disconnect(this);
}

QSettings *PPSettings::internalSettings() const
{
    return m_internal->internalSettings();
}

bool PPSettings::setMusicDir(const QStringList &dirList)
{
    return m_internal->setMusicDir(dirList);
}

bool PPSettings::addMusicDir(const QString &dir)
{
    return m_internal->addMusicDir(dir);
}

bool PPSettings::deleteMusicDir(const QString &target)
{
    return m_internal->deleteMusicDir(target);
}

const QString PPSettings::defaultMusicDir() const
{
    return m_internal->defaultMusicDir();
}

QStringList PPSettings::musicDirs()
{
    return m_internal->musicDirs();
}

void PPSettings::setLastPlayedSong(const AudioMetaObject &data)
{
    m_internal->setLastPlayedSong(data);
}

AudioMetaObject PPSettings::lastPlayedSong() const
{
    return m_internal->lastPlayedSong();
}

bool PPSettings::setCurrentPlayListHash(const QString &hash)
{
    return m_internal->setCurrentPlayListHash(hash);
}

QString PPSettings::getPlayListHash()
{
    return m_internal->getPlayListHash();
}

bool PPSettings::setMusicImageCachePath(const QString &absolutePath)
{
    return m_internal->setMusicImageCachePath(absolutePath);
}

QString PPSettings::musicImageCachePath()
{
    return m_internal->musicImageCachePath();
}

void PPSettings::setTraceLog(bool trace)
{
    m_internal->setTraceLog(trace);
}

bool PPSettings::traceLog()
{
    return m_internal->traceLog();
}

void PPSettings::setConfig(const QString &key, const QString &value)
{
    m_internal->setConfig(key, value);
}

QString PPSettings::getConfig(const QString &key, const QString &defaultValue)
{
    return m_internal->getConfig(key, defaultValue);
}

void PPSettings::setConfig(const QString &key, bool value)
{
    m_internal->setConfig(key, value);
}

bool PPSettings::getConfig(const QString &key, bool defaultValue)
{
    return m_internal->getConfig(key, defaultValue);
}

bool PPSettings::autoFetchMetaData()
{
    return m_internal->autoFetchMetaData();
}

void PPSettings::setAutoFetchMetaData(bool autoFetch)
{
    m_internal->setAutoFetchMetaData(autoFetch);
}

bool PPSettings::fetchMetadataOnMobileNetwork()
{
    return m_internal->fetchMetadataOnMobileNetwork();
}

void PPSettings::setFetchMetadataOnMobileNetwork(bool fetch)
{
    m_internal->setFetchMetadataOnMobileNetwork(fetch);
}

QString PPSettings::playListDir() const
{
    return m_internal->playListDir();
}

QString PPSettings::playListDBPath() const
{
    return m_internal->playListDBPath();
}

QString PPSettings::curPlayBackend() const
{
    return m_internal->curPlayBackend();
}

QString PPSettings::curMusicLibraryDAO() const
{
    return m_internal->curMusicLibraryDAO();
}

QString PPSettings::curOutPut() const
{
    return m_internal->curOutPut();
}

void PPSettings::enableDecoder(const QString &libraryFile)
{
    m_internal->enableDecoder(libraryFile);
}

void PPSettings::disableDecoder(const QString &libraryFile)
{
    m_internal->disableDecoder(libraryFile);
}

QStringList PPSettings::decoderLibraries() const
{
    return  m_internal->decoderLibraries();
}

void PPSettings::enableMetadataLookup(const QString &libraryFile)
{
    m_internal->enableMetadataLookup(libraryFile);
}

void PPSettings::disableMetadataLookup(const QString &libraryFile)
{
    m_internal->disableMetadataLookup(libraryFile);
}

QStringList PPSettings::metadataLookupLibraries() const
{
    return m_internal->metadataLookupLibraries();
}

void PPSettings::enableTagParser(const QString &libraryFile)
{
    m_internal->enableTagParser(libraryFile);
}

void PPSettings::disableTagParser(const QString &libraryFile)
{
    m_internal->disableTagParser(libraryFile);
}

QStringList PPSettings::tagParserLibraries() const
{
    return m_internal->tagParserLibraries();
}

void PPSettings::enableSpectrumGenerator(const QString &lib)
{
    m_internal->enableSpectrumGenerator(lib);
}

void PPSettings::disableSpectrumGenerator(const QString &lib)
{
    m_internal->disableSpectrumGenerator(lib);
}

QStringList PPSettings::spectrumGeneratorLibraries() const
{
    return m_internal->spectrumGeneratorLibraries();
}

void PPSettings::setPlayListDir(QString arg)
{
    m_internal->setPlayListDir(arg);
}

void PPSettings::setCurPlayBackend(const QString &libraryFile)
{
    m_internal->setCurPlayBackend(libraryFile);
}

void PPSettings::setCurMusicLibraryDAO(const QString &libraryFile)
{
    m_internal->setCurMusicLibraryDAO(libraryFile);
}

void PPSettings::setCurOutPut(const QString &libraryFile)
{
    m_internal->setCurOutPut(libraryFile);
}

} //PhoenixPlayer
