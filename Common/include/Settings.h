#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>

class QSettings;

namespace PhoenixPlayer {

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
//    static Settings *getInstance();
    virtual ~Settings();

    Q_INVOKABLE bool setMusicDir(const QStringList &dirList);
    Q_INVOKABLE QStringList getMusicDirs();

    Q_INVOKABLE bool setLastPlayedSong(const QString &songHash);
    Q_INVOKABLE QString getLastPlayedSong();

    Q_INVOKABLE bool setCurrentPlayListHash(const QString &hash);
    Q_INVOKABLE QString getPlayListHash();

    Q_INVOKABLE bool setPlayBackend(const QString &backendName);
    Q_INVOKABLE QString getCurrentPlayBackend();

    Q_INVOKABLE bool setMusicImageCachePath(const QString &absolutePath);
    Q_INVOKABLE QString getMusicImageCachePath();
signals:

public slots:

private:

private:
    QSettings *mSettings;

    QString mDefaultMusicDir;
    QString mDefualtMusicImageDir;
};
} //PhoenixPlayer
#endif // SETTINGS_H
