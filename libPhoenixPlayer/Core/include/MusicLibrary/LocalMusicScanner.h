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
class FileListScanner;
class LocalMusicScannerThread;
class AudioParser;
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
    void searchingDir(const QString &dirName);
    void parsingFile(const QString &file, int remainingSize);
    void searchingFinished();

private:
    void doScann(const QString &dirname);
private:
    PPSettings                  *m_settings         = Q_NULLPTR;
    PluginLoader                *m_pluginLoader     = Q_NULLPTR;
    FileListScanner             *m_fileListScanner  = Q_NULLPTR;
    AudioParser                 *m_audioParser      = Q_NULLPTR;
};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNER_H
