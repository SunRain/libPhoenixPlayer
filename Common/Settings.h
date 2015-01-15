#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;
class QVariant;
class QStringList;

namespace PhoenixPlayer {

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings *getInstance();
    virtual ~Settings();

    bool setMusicDir(const QStringList &dirList);
    QStringList getMusicDirs();
signals:

public slots:

private:
    explicit Settings(QObject *parent = 0);

private:
    QSettings *mSettings;

    QString mDefaultMusicDir;
};
} //PhoenixPlayer
#endif // SETTINGS_H
