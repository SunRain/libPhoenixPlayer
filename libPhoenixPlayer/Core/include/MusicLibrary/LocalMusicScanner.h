#ifndef LOCALMUSICSCANNER_H
#define LOCALMUSICSCANNER_H

#include <QObject>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class PPSettings;
class PluginLoader;
class AudioMetaObject;
namespace MusicLibrary {

class IMusicLibraryDAO;
class LocalMusicScannerThread;
class LIBPHOENIXPLAYER_EXPORT LocalMusicScanner : public QObject
{
    Q_OBJECT
    friend class MusicLibraryManager;
protected:
    explicit LocalMusicScanner(PPSettings *set, PluginLoader *loader, QObject *parent = Q_NULLPTR);
    virtual ~LocalMusicScanner();

public:
    void scanLocalMusic();
    void scanDir(const QString &dirname);
    void scarnDirs(const QStringList &list);

signals:
    void searchingFinished();

private:
    void doScann(const QString &dirname);
private:
    PPSettings                  *m_settings;
    PluginLoader                *m_pluginLoader;
    LocalMusicScannerThread     *m_scanner;
};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNER_H
