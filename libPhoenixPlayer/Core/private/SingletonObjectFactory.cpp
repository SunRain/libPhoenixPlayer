#include "SingletonObjectFactory.h"

#include <QDebug>

#include "SingletonPointer.h"
#include "PPSettingsInternal.h"
#include "PluginMgrInternal.h"
#include "MusicLibraryManagerInternal.h"
#include "LocalMusicScannerInternal.h"
#include "VolumeControlInternal.h"
#include "PlayerCoreInternal.h"
#include "EqualizerMgrInternal.h"
#include "UserInterfaceMgrInternal.h"
#include "PlayListMetaMgrInternal.h"

namespace PhoenixPlayer {

#define SINGLETON SingletonObjectFactory::instance()

SingletonObjectFactory::SingletonObjectFactory(QObject *parent)
    : QObject(parent)
{

}

SingletonObjectFactory *SingletonObjectFactory::createInstance()
{
    return new SingletonObjectFactory();
}

SingletonObjectFactory::~SingletonObjectFactory()
{

}

SingletonObjectFactory *SingletonObjectFactory::instance()
{
    return Singleton<SingletonObjectFactory>::instance(SingletonObjectFactory::createInstance);
}

void SingletonObjectFactory::initSettings()
{
    if (m_setings.isNull()) {
        m_setings = QSharedPointer<PPSettingsInternal>(new PPSettingsInternal, [](PPSettingsInternal *obj) {
            qDebug()<<Q_FUNC_INFO<<"--- Delete PPSettingsInternal";
            obj->deleteLater();
        });
    }
}

void SingletonObjectFactory::initPluginMgr()
{
    if (m_plugin.isNull()) {
        m_plugin = QSharedPointer<PluginMgrInternal>(new PluginMgrInternal(SINGLETON->settingsInternal()),
                                                     [](PluginMgrInternal *obj) {
            qDebug()<<Q_FUNC_INFO<<"--- Delete PluginMgrInternal";
            obj->deleteLater();
        });
    }
}

void SingletonObjectFactory::initMusicLibMgr()
{
    if (m_musicLib.isNull()) {
        m_musicLib = QSharedPointer<MusicLibraryManagerInternal>(new MusicLibraryManagerInternal(SINGLETON->settingsInternal(),
                                                                                                 SINGLETON->pluginMgrInternal()),
                                                                 [](MusicLibraryManagerInternal *obj) {
                                                                     qDebug()<<Q_FUNC_INFO<<"--- Delete MusicLibraryManagerInternal";
                                                                     obj->deleteLater();
                                                                 });
    }
}

void SingletonObjectFactory::initMusicScanner()
{
    if (m_localMC.isNull()) {
        m_localMC = QSharedPointer<LocalMusicScannerInternal>(new LocalMusicScannerInternal(SINGLETON->pluginMgrInternal()),
                                                              [](LocalMusicScannerInternal *obj) {
                                                                  qDebug()<<Q_FUNC_INFO<<"--- Delete LocalMusicScannerInternal";
                                                                  obj->deleteLater();
                                                              });

    }
}

void SingletonObjectFactory::initVolCtrl()
{
     if (m_volCtrl.isNull()) {
        m_volCtrl = QSharedPointer<VolumeControlInternal>(new VolumeControlInternal(SINGLETON->pluginMgrInternal()),
                                                          [](VolumeControlInternal *obj) {
                                                              qDebug()<<Q_FUNC_INFO<<"--- Delete VolumeControlInternal";
                                                              obj->deleteLater();
                                                          });
    }
}

void SingletonObjectFactory::initPlayCore()
{
    if (m_playCore.isNull()) {
        m_playCore = QSharedPointer<PlayerCoreInternal>(new PlayerCoreInternal(SINGLETON->pluginMgrInternal(),
                                                                               SINGLETON->musicLibraryManagerInternal()),
                                                        [](PlayerCoreInternal *obj) {
                                                            qDebug()<<Q_FUNC_INFO<<"--- Delete PlayerCoreInternal";
                                                            obj->deleteLater();
                                                        });
    }
}

void SingletonObjectFactory::initEQMgr()
{
    if (m_eq.isNull()) {
        m_eq = QSharedPointer<EqualizerMgrInternal>(new EqualizerMgrInternal(SINGLETON->settingsInternal()),
                                                    [](EqualizerMgrInternal *obj) {
                                                        qDebug()<<Q_FUNC_INFO<<"--- Delete EqualizerMgrInternal";
                                                        obj->deleteLater();
                                                    });
    }
}

void SingletonObjectFactory::initUserInterfaceInternal()
{
    if (m_ui.isNull()) {
        m_ui = QSharedPointer<UserInterfaceMgrInternal>(new UserInterfaceMgrInternal(SINGLETON->pluginMgrInternal()),
                                                        [](UserInterfaceMgrInternal *obj) {
                                                           qDebug()<<Q_FUNC_INFO<<"--- Delete UserInterfaceMgrInternal";
                                                           obj->deleteLater();
                                                        });
    }
}

void SingletonObjectFactory::initPlayListMetaMgrInternal()
{
    if (m_plsMgr.isNull()) {
        m_plsMgr = QSharedPointer<PlayListMetaMgrInternal>(new PlayListMetaMgrInternal(SINGLETON->settingsInternal()),
                                                           [](PlayListMetaMgrInternal *obj) {
                                                              qDebug()<<Q_FUNC_INFO<<"--- Delete PlayListMetaMgrInternal";
                                                              obj->saveToDatabase();
                                                              obj->deleteLater();
                                                           });
    }
}


} //PhoenixPlayer
