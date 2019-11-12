#ifndef SINGLETONOBJECTFACTORY_H
#define SINGLETONOBJECTFACTORY_H

#include <QObject>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class PPSettingsInternal;
class PluginMgrInternal;
class MusicLibraryManagerInternal;
class LocalMusicScannerInternal;
class VolumeControlInternal;
class PlayerCoreInternal;
class EqualizerMgrInternal;
class UserInterfaceMgrInternal;
class PlayListMetaMgrInternal;

class LIBPHOENIXPLAYER_EXPORT SingletonObjectFactory : public QObject
{
    Q_OBJECT
private:
    explicit SingletonObjectFactory(QObject *parent = Q_NULLPTR);
    static SingletonObjectFactory *createInstance();

public:
    virtual ~SingletonObjectFactory();
    static SingletonObjectFactory *instance();

    inline QSharedPointer<PPSettingsInternal> settingsInternal()
    {
        initSettings();
        return m_setings;
    }

    inline QSharedPointer<PluginMgrInternal> pluginMgrInternal()
    {
        initPluginMgr();
        return m_plugin;
    }

    inline QSharedPointer<MusicLibraryManagerInternal> musicLibraryManagerInternal()
    {
        initMusicLibMgr();
        return m_musicLib;
    }

    inline QSharedPointer<LocalMusicScannerInternal> localMusicScannerInternal()
    {
        initMusicScanner();
        return m_localMC;
    }

    inline QSharedPointer<VolumeControlInternal> volumeControlInternal()
    {
        initVolCtrl();
        return m_volCtrl;
    }

    inline QSharedPointer<PlayerCoreInternal> playerCoreInternal()
    {
        initPlayCore();
        return m_playCore;
    }

    inline QSharedPointer<EqualizerMgrInternal> eqMgrInternal()
    {
        initEQMgr();
        return m_eq;
    }

    inline QSharedPointer<UserInterfaceMgrInternal> userInterfaceInternal()
    {
        initUserInterfaceInternal();
        return m_ui;
    }

    inline QSharedPointer<PlayListMetaMgrInternal> plsMgrInternal()
    {
        initPlayListMetaMgrInternal();
        return m_plsMgr;
    }

private:
    void initSettings();

    void initPluginMgr();

    void initMusicLibMgr();

    void initMusicScanner();

    void initVolCtrl();

    void initPlayCore();

    void initEQMgr();

    void initUserInterfaceInternal();

    void initPlayListMetaMgrInternal();

private:
    QSharedPointer<PPSettingsInternal>          m_setings;
    QSharedPointer<PluginMgrInternal>           m_plugin;
    QSharedPointer<MusicLibraryManagerInternal> m_musicLib;
    QSharedPointer<LocalMusicScannerInternal>   m_localMC;
    QSharedPointer<VolumeControlInternal>       m_volCtrl;
    QSharedPointer<PlayerCoreInternal>          m_playCore;
    QSharedPointer<EqualizerMgrInternal>        m_eq;
    QSharedPointer<UserInterfaceMgrInternal>    m_ui;
    QSharedPointer<PlayListMetaMgrInternal>     m_plsMgr;
};


} //PhoenixPlayer
#endif // SINGLETONOBJECTFACTORY_H
