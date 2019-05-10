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
public:
    explicit LocalMusicScanner(PPSettings *set, QObject *parent = 0);
    explicit LocalMusicScanner(QObject *parent = 0);
    virtual ~LocalMusicScanner();
    Q_INVOKABLE void scanLocalMusic();
    Q_INVOKABLE void scanDir(const QString &dirname);

signals:
    void searchingFinished();

private:
    void doScann(const QString &dirname);
private:
    PPSettings *m_settings;
    LocalMusicScannerThread *m_scanner;
};
} //MusicLibrary
} //PhoenixPlayer

#endif // LOCALMUSICSCANNER_H
