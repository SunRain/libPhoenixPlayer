#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QStringList>

#include "Settings.h"

namespace PhoenixPlayer {

const char *KEY_MUSIC_DIR = "MusicDir";

Settings::Settings(QObject *parent) : QObject(parent)
{
    mDefaultMusicDir = QString("%1/%2").arg (QDir::homePath ())
            .arg(QStandardPaths::displayName (QStandardPaths::MusicLocation));

    mSettings = new QSettings(qApp->organizationName(), qApp->applicationName());
}

Settings *Settings::getInstance()
{
    static Settings s;
    return &s;
}

Settings::~Settings()
{
    mSettings->deleteLater ();
}

bool Settings::setMusicDir(const QStringList &dirList)
{
    if (dirList.isEmpty ())
        return false;

    QStringList list = dirList;
    list.removeDuplicates ();
    QString last = list.takeLast ();
    if (list.isEmpty ()) {
        mSettings->setValue (KEY_MUSIC_DIR, last);
        return true;
    }
    QString tmp;
    foreach (QString s, list) {
        tmp = QString("%1||").arg (s);
    }
    mSettings->setValue (KEY_MUSIC_DIR, QString("%1%2").arg (tmp).arg (last));
    return true;
}

QStringList Settings::getMusicDirs()
{
    return mSettings->value (KEY_MUSIC_DIR, mDefaultMusicDir).toStringList ();
}
} //PhoenixPlayer
