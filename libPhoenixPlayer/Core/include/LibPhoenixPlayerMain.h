#ifndef LIBPHOENIXPLAYERMAIN_H
#define LIBPHOENIXPLAYERMAIN_H

#include <QObject>

#include "libphoenixplayer_global.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {

class Settings;
class MusicLibraryManager;
class VolumeControl;
class PlayerCore;
class MetadataLookupMgrWrapper;

class LibPhoenixPlayer : public QObject
{
    Q_OBJECT

    DECLARE_SINGLETON_POINTER(LibPhoenixPlayer)
public:
    virtual ~LibPhoenixPlayer();
    void initiate();

    Settings *settings();
    MusicLibraryManager *libraryMgr();
    VolumeControl *volumeCtrl();
    PlayerCore *playerCore();
    MetadataLookupMgrWrapper *metaLookupMgr();

private:
    bool m_initiated;
    Settings *m_settings;
    MusicLibraryManager *m_libraryMgr;
    VolumeControl *m_volumeCtrl;
    PlayerCore *m_playerCore;
    MetadataLookupMgrWrapper *m_lookupMgr;
};

} //PhoenixPlayer
#endif // LIBPHOENIXPLAYERMAIN_H
