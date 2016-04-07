#ifndef LIBPHOENIXPLAYERMAIN_H
#define LIBPHOENIXPLAYERMAIN_H

#include <QObject>
#include <QDebug>

#include "libphoenixplayer_global.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {

class Settings;
class VolumeControl;
class PlayerCore;
class PluginLoader;

namespace MusicLibrary {
class MusicLibraryManager;
class LocalMusicScanner;
}

namespace MetadataLookup {
class MetadataLookupMgrWrapper;
}

class LIBPHOENIXPLAYER_EXPORT LibPhoenixPlayer : public QObject
{
    Q_OBJECT

    DECLARE_SINGLETON_POINTER(LibPhoenixPlayer)
public:
    explicit LibPhoenixPlayer(QObject *parent = 0);
    virtual ~LibPhoenixPlayer();
    void initiate();
    void registerPlugins(const char *url);

    Settings *settings();
    MusicLibrary::MusicLibraryManager *libraryMgr();
    VolumeControl *volumeCtrl();
    PlayerCore *playerCore();
    MetadataLookup::MetadataLookupMgrWrapper *metaLookupMgr();
    PluginLoader *pluginLoader();

private:
    inline void checkInitiate() {
        if (!m_initiated)
            qFatal("[LibPhoenixPlayerMain]: initiate() should be called first !!!");
    }
private:
    bool m_initiated;
    Settings *m_settings;
    MusicLibrary::MusicLibraryManager *m_libraryMgr;
    VolumeControl *m_volumeCtrl;
    PlayerCore *m_playerCore;
    MetadataLookup::MetadataLookupMgrWrapper *m_lookupMgr;
    PluginLoader *m_pluginLoader;
//    MusicLibrary::LocalMusicScanner *m_localScanner;
};

} //PhoenixPlayer
#endif // LIBPHOENIXPLAYERMAIN_H
