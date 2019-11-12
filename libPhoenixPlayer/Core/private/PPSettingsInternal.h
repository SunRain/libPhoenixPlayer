#ifndef PPSETTINGSINTERNAL_H
#define PPSETTINGSINTERNAL_H

#include <QObject>
#include <QSettings>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {


class LIBPHOENIXPLAYER_EXPORT PPSettingsInternal : public QObject
{
    Q_OBJECT
public:
    explicit PPSettingsInternal(QObject *parent = Q_NULLPTR);
    virtual ~PPSettingsInternal();

    inline QSettings *internalSettings() const
    {
        return m_settings;
    }

    bool setMusicDir(const QStringList &dirList);
    bool addMusicDir(const QString &dir);
    bool deleteMusicDir(const QString &target);
    inline const QString defaultMusicDir() const
    {
        return m_defaultMusicDir;
    }
    QStringList musicDirs();


    void setLastPlayedSong(const AudioMetaObject &data);
    AudioMetaObject lastPlayedSong() const;

    bool setCurrentPlayListHash(const QString &hash);
    QString getPlayListHash();

    bool setMusicImageCachePath(const QString &absolutePath);
    QString musicImageCachePath();

    void setTraceLog(bool trace);
    bool traceLog();

    void setConfig(const QString &key, const QString &value);
    QString getConfig(const QString &key, const QString &defaultValue = QString());

    void setConfig (const QString &key, bool value);
    bool getConfig (const QString &key, bool defaultValue);

    inline bool autoFetchMetaData()
    {
        return m_autoFetchMetadata;
    }
    void setAutoFetchMetaData(bool autoFetch);

    inline bool fetchMetadataOnMobileNetwork()
    {
        return m_fetchMetaDataMobileNetwork;
    }
    void setFetchMetadataOnMobileNetwork(bool fetch);

    QString playListDir() const;

    inline QString playListDBPath() const
    {
        return m_playListDBPath;
    }

    QString curPlayBackend() const;

    QString curMusicLibraryDAO() const;

    QString curOutPut() const;

    void enableDecoder(const QString &libraryFile);
    void disableDecoder(const QString &libraryFile);
    QStringList decoderLibraries() const;

    void enableMetadataLookup(const QString &libraryFile);
    void disableMetadataLookup(const QString &libraryFile);
    QStringList metadataLookupLibraries() const;

    void enableTagParser(const QString &libraryFile);
    void disableTagParser(const QString &libraryFile);
    QStringList tagParserLibraries() const;

    void enableSpectrumGenerator(const QString &lib);
    void disableSpectrumGenerator(const QString &lib);
    QStringList spectrumGeneratorLibraries() const;

    void setPlayListDir(QString arg);
    void setCurPlayBackend(const QString &libraryFile);
    void setCurMusicLibraryDAO(const QString &libraryFile);
    void setCurOutPut(const QString &libraryFile);

signals:
    void autoFetchMetaDataChanged();
    void fetchMetaDataMobileNetworkChanged();
    void playListDirChanged(const QString &arg);
    void curPlayBackendChanged(const QString &libraryFile);
    void curMusicLibraryDAOChanged(const QString &libraryFile);
    void curOutPutChanged(const QString &libraryFile);

private:
    void checkInit();

private:
    QSettings   *m_settings                     = Q_NULLPTR;

    bool        m_autoFetchMetadata             = true;
    bool        m_fetchMetaDataMobileNetwork    = false;

    QString     m_defaultMusicDir;
    QString     m_defaultMusicImageDir;
    QString     m_defaultPlayListDir;
    QString     m_playListDBPath;

};

} //namespace PhoenixPlayer
#endif // PPSETTINGSINTERNAL_H
