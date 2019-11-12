#ifndef PPSETTINGS_H
#define PPSETTINGS_H

#include <QObject>
#include <QStringList>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"

class QSettings;

namespace PhoenixPlayer {

class AudioMetaObject;
class PPSettingsInternal;

/*!
 * \brief The PPSettings class
 * Internal Singleton class
 */
class PPSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoFetchMetaData READ autoFetchMetaData WRITE setAutoFetchMetaData NOTIFY autoFetchMetaDataChanged)
    Q_PROPERTY(bool fetchMetadataOnMobileNetwork READ fetchMetadataOnMobileNetwork  WRITE setFetchMetadataOnMobileNetwork  NOTIFY fetchMetaDataMobileNetworkChanged)
    Q_PROPERTY(QString playListDir READ playListDir WRITE setPlayListDir NOTIFY playListDirChanged)

    Q_PROPERTY(QString curPlayBackend READ curPlayBackend WRITE setCurPlayBackend NOTIFY curPlayBackendChanged)
    Q_PROPERTY(QString curMusicLibraryDAO READ curMusicLibraryDAO WRITE setCurMusicLibraryDAO NOTIFY curMusicLibraryDAOChanged)
    Q_PROPERTY(QString curOutPut READ curOutPut WRITE setCurOutPut NOTIFY curOutPutChanged)

public:
    explicit PPSettings(QObject *parent = Q_NULLPTR);
    virtual ~PPSettings();

    QSettings *internalSettings() const;

    Q_INVOKABLE bool setMusicDir(const QStringList &dirList);
    Q_INVOKABLE bool addMusicDir(const QString &dir);
    Q_INVOKABLE bool deleteMusicDir(const QString &target);
    Q_INVOKABLE const QString defaultMusicDir() const;
    Q_INVOKABLE QStringList musicDirs();

    void setLastPlayedSong(const AudioMetaObject &data);
    AudioMetaObject lastPlayedSong() const;

    Q_INVOKABLE bool setCurrentPlayListHash(const QString &hash);
    Q_INVOKABLE QString getPlayListHash();

    Q_INVOKABLE bool setMusicImageCachePath(const QString &absolutePath);
    Q_INVOKABLE QString musicImageCachePath();

    Q_INVOKABLE void setTraceLog(bool trace);
    Q_INVOKABLE bool traceLog();

    Q_INVOKABLE void setConfig(const QString &key, const QString &value);
    Q_INVOKABLE QString getConfig(const QString &key, const QString &defaultValue = QString());

    Q_INVOKABLE void setConfig (const QString &key, bool value);
    Q_INVOKABLE bool getConfig (const QString &key, bool defaultValue);

    bool autoFetchMetaData();
    void setAutoFetchMetaData(bool autoFetch);

    bool fetchMetadataOnMobileNetwork();
    void setFetchMetadataOnMobileNetwork(bool fetch);

    QString playListDir() const;

    QString playListDBPath() const;

    ///
    /// \brief curPlayBackend
    /// \return current playBackend library file
    ///
    QString curPlayBackend() const;

    ///
    /// \brief curMusicLibraryDAO
    /// \return  current music library DAO library file
    ///
    QString curMusicLibraryDAO() const;

    ///
    /// \brief curOutPut
    /// \return current output library file
    ///
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

signals:
    void autoFetchMetaDataChanged();
    void fetchMetaDataMobileNetworkChanged();
    void playListDirChanged(const QString &arg);
    void curPlayBackendChanged(const QString &libraryFile);
    void curMusicLibraryDAOChanged(const QString &libraryFile);
    void curOutPutChanged(const QString &libraryFile);

public slots:
    void setPlayListDir(QString arg);
    void setCurPlayBackend(const QString &libraryFile);
    void setCurMusicLibraryDAO(const QString &libraryFile);
    void setCurOutPut(const QString &libraryFile);

private:
    QSharedPointer<PPSettingsInternal> m_internal;

};
} //PhoenixPlayer
#endif // PPSETTINGS_H
