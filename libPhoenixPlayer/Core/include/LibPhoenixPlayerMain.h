//#ifndef LIBPHOENIXPLAYERMAIN_H
//#define LIBPHOENIXPLAYERMAIN_H

//#include <QObject>
//#include <QDebug>

//#include "libphoenixplayer_global.h"
//#include "SingletonPointer.h"

//#define phoenixPlayerLib PhoenixPlayer::LibPhoenixPlayer::instance()

//namespace PhoenixPlayer {

//class PPSettings;
//class VolumeControl;
//class PlayerCore;
//class PluginLoader;

//namespace MusicLibrary {
//class MusicLibraryManager;
//class LocalMusicScanner;
//}

//namespace MetadataLookup {
//class MetadataLookupMgrWrapper;
//}

//class LIBPHOENIXPLAYER_EXPORT LibPhoenixPlayer : public QObject
//{
//    Q_OBJECT
//private:
//    explicit LibPhoenixPlayer(QObject *parent = Q_NULLPTR);
//    static LibPhoenixPlayer *createInstance();

//public:
//    static LibPhoenixPlayer *instance();
//    virtual ~LibPhoenixPlayer();
//    void initiate();
//    void registerPlugins(const char *url);

//    PPSettings *settings();
//    MusicLibrary::MusicLibraryManager *libraryMgr();
//    VolumeControl *volumeCtrl();
//    PlayerCore *playerCore();
//    MetadataLookup::MetadataLookupMgrWrapper *metaLookupMgr();
//    PluginLoader *pluginLoader();

//private:
//    inline void checkInitiate() {
//        if (!m_initiated)
//            qFatal("[LibPhoenixPlayerMain]: initiate() should be called first !!!");
//    }
//private:
//    bool m_initiated;
//    PPSettings *m_settings;
//    MusicLibrary::MusicLibraryManager *m_libraryMgr;
//    VolumeControl *m_volumeCtrl;
//    PlayerCore *m_playerCore;
//    MetadataLookup::MetadataLookupMgrWrapper *m_lookupMgr;
//    PluginLoader *m_pluginLoader;
////    MusicLibrary::LocalMusicScanner *m_localScanner;
//};

//} //PhoenixPlayer
//#endif // LIBPHOENIXPLAYERMAIN_H
