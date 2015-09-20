#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>
#include "SingletonPointer.h"

class QSettings;

namespace PhoenixPlayer {

class SongMetaData;
class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoFetchMetaData READ autoFetchMetaData WRITE setAutoFetchMetaData NOTIFY autoFetchMetaDataChanged)
    Q_PROPERTY(bool fetchMetadataOnMobileNetwork READ fetchMetadataOnMobileNetwork  WRITE setFetchMetadataOnMobileNetwork  NOTIFY fetchMetaDataMobileNetworkChanged)
    Q_PROPERTY(QString playListDir READ playListDir WRITE setPlayListDir NOTIFY playListDirChanged)

    Q_PROPERTY(QString curPlayBackend READ curPlayBackend WRITE setCurPlayBackend NOTIFY curPlayBackendChanged)
    Q_PROPERTY(QString curMusicLibraryDAO READ curMusicLibraryDAO WRITE setCurMusicLibraryDAO NOTIFY curMusicLibraryDAOChanged)
    Q_PROPERTY(QString curOutPut READ curOutPut WRITE setCurOutPut NOTIFY curOutPutChanged)
    DECLARE_SINGLETON_POINTER(Settings)
public:
//    static Settings *getInstance();
    virtual ~Settings();

    QSettings *settings() const;

    Q_INVOKABLE bool setMusicDir(const QStringList &dirList);
    Q_INVOKABLE bool addMusicDir(const QString &dir);
    Q_INVOKABLE bool deleteMusicDir(const QString &target);
    Q_INVOKABLE QStringList musicDirs();

//    Q_INVOKABLE bool setLastPlayedSong(const QString &songHash);
//    Q_INVOKABLE QString lastPlayedSong();
    void setLastPlayedSong(SongMetaData **data);
    SongMetaData *lastPlayedSong();

    Q_INVOKABLE bool setCurrentPlayListHash(const QString &hash);
    Q_INVOKABLE QString getPlayListHash();

//    Q_INVOKABLE bool setPlayBackend(const QString &backendName);
//    Q_INVOKABLE QString getCurrentPlayBackend();


    Q_INVOKABLE bool setMusicImageCachePath(const QString &absolutePath);
    Q_INVOKABLE QString musicImageCachePath();

    Q_INVOKABLE bool setTraceLog(bool trace);
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

    void enableTagPaser(const QString &libraryFile);
    void disableTagPaser(const QString &libraryFile);
    QStringList tagPaserLibraries() const;

signals:
    void autoFetchMetaDataChanged();
    void fetchMetaDataMobileNetworkChanged();
    void playListDirChanged(QString arg);
    void curPlayBackendChanged(QString libraryFile);
    void curMusicLibraryDAOChanged(QString libraryFile);
    void curOutPutChanged(QString libraryFile);

public slots:
    void setPlayListDir(QString arg);
    void setCurPlayBackend(const QString &libraryFile);
    void setCurMusicLibraryDAO(const QString &libraryFile);
    void setCurOutPut(const QString &libraryFile);

private:
    void checkInit();
private:
    QSettings *m_settings;

    QString m_defaultMusicDir;
    QString m_defaultMusicImageDir;
    bool m_autoFetchMetadata;
    bool m_fetchMetaDataMobileNetwork;
    QString m_defaultPlayListDir;
    //    QString m_curPlayBackend;
    //    QString m_curMusicLibraryDAO;
//    QString m_curOutPut;
};
} //PhoenixPlayer
#endif // SETTINGS_H
