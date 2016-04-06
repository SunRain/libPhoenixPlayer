#ifndef LIBPHOENIXPLAYERMAIN_H
#define LIBPHOENIXPLAYERMAIN_H

#include <QObject>

#include "libphoenixplayer_global.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {

class Settings;
class VolumeControl;
class PlayerCore;

namespace MusicLibrary {
class MusicLibraryManager;
}

namespace MetadataLookup {
class MetadataLookupMgrWrapper;
}

class LIBPHOENIXPLAYER_EXPORT LibPhoenixPlayer : public QObject
{
    Q_OBJECT

    DECLARE_SINGLETON_POINTER(LibPhoenixPlayer)
public:
    virtual ~LibPhoenixPlayer();
    void initiate();

    Settings *settings();
    MusicLibrary::MusicLibraryManager *libraryMgr();
    VolumeControl *volumeCtrl();
    PlayerCore *playerCore();
    MetadataLookup::MetadataLookupMgrWrapper *metaLookupMgr();

private:
    bool m_initiated;
    Settings *m_settings;
    MusicLibrary::MusicLibraryManager *m_libraryMgr;
    VolumeControl *m_volumeCtrl;
    PlayerCore *m_playerCore;
    MetadataLookup::MetadataLookupMgrWrapper *m_lookupMgr;
};

} //PhoenixPlayer
#endif // LIBPHOENIXPLAYERMAIN_H
