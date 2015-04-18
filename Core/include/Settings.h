#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>

class QSettings;

namespace PhoenixPlayer {

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoFetchMetaData READ autoFetchMetaData WRITE setAutoFetchMetaData NOTIFY autoFetchMetaDataChanged)
public:
    explicit Settings(QObject *parent = 0);
//    static Settings *getInstance();
    virtual ~Settings();

#ifdef SAILFISH_OS
    static Settings *instance();
#endif

    Q_INVOKABLE bool setMusicDir(const QStringList &dirList);
    Q_INVOKABLE bool addMusicDir(const QString &dir);
    Q_INVOKABLE bool deleteMusicDir(const QString &target);
    Q_INVOKABLE QStringList getMusicDirs();

    Q_INVOKABLE bool setLastPlayedSong(const QString &songHash);
    Q_INVOKABLE QString getLastPlayedSong();

    Q_INVOKABLE bool setCurrentPlayListHash(const QString &hash);
    Q_INVOKABLE QString getPlayListHash();

    Q_INVOKABLE bool setPlayBackend(const QString &backendName);
    Q_INVOKABLE QString getCurrentPlayBackend();

    Q_INVOKABLE bool setMusicImageCachePath(const QString &absolutePath);
    Q_INVOKABLE QString getMusicImageCachePath();

    Q_INVOKABLE bool setTraceLog(bool trace);
    Q_INVOKABLE bool traceLog();

    bool autoFetchMetaData();
    void setAutoFetchMetaData(bool autoFetch);

signals:
    void autoFetchMetaDataChanged();
public slots:

private:
    void checkInit();
private:
    QSettings *mSettings;

    QString mDefaultMusicDir;
    QString mDefaultMusicImageDir;
    bool mAutoFetchMetadata;
};
} //PhoenixPlayer
#endif // SETTINGS_H
