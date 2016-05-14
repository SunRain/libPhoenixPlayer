#include "LibPhoenixPlayerMain.h"

#include <QtQml>
#include <QDebug>

#include "Settings.h"
#include "PluginLoader.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "PlayerCore/VolumeControl.h"
#include "PlayerCore/PlayerCore.h"
#include "MusicLibrary/LocalMusicScanner.h"
#include "AudioMetaObjectKeys.h"

#include "Common.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;
using namespace MetadataLookup;

LibPhoenixPlayer::LibPhoenixPlayer(QObject *parent)
    : QObject(parent)
{

    //TODO do not use PluginLoader as paramater, this may confront with initiate order problem
    m_settings = new Settings();
    m_pluginLoader = new PluginLoader(m_settings);
    m_libraryMgr = new MusicLibraryManager(m_settings, m_pluginLoader);
    m_volumeCtrl = new VolumeControl(m_pluginLoader);
    m_playerCore = new PlayerCore(m_settings, m_pluginLoader, m_libraryMgr);
    m_initiated = true;
}

LibPhoenixPlayer::~LibPhoenixPlayer()
{

}

void LibPhoenixPlayer::initiate()
{
    qDebug()<<">>>>>>>>>>>>>>>>>>>>>>"<<Q_FUNC_INFO<<"<<<<<<<<<<<<<";
    m_playerCore->initiate ();
}

void LibPhoenixPlayer::registerPlugins(const char *url)
{
    checkInitiate ();

    qmlRegisterUncreatableType<Common>(url, 1, 0, "Common", "Common cannot be created");
    qmlRegisterSingletonType<AudioMetaObjectKeyName>(url, 1, 0, "MetaKey", AudioMetaObjectKeyName::qmlSingleton);

    qmlRegisterType<LocalMusicScanner>(url, 1, 0, "LocalMusicScanner");
}

Settings *LibPhoenixPlayer::settings()
{
    checkInitiate ();
    return m_settings;
}

MusicLibraryManager *LibPhoenixPlayer::libraryMgr()
{
    checkInitiate ();
    return m_libraryMgr;
}

VolumeControl *LibPhoenixPlayer::volumeCtrl()
{
    checkInitiate ();
    return m_volumeCtrl;
}

PlayerCore *LibPhoenixPlayer::playerCore()
{
    checkInitiate ();
    return m_playerCore;
}

MetadataLookupMgrWrapper *LibPhoenixPlayer::metaLookupMgr()
{
    checkInitiate ();
    return m_lookupMgr;
}

PluginLoader *LibPhoenixPlayer::pluginLoader()
{
    checkInitiate ();
    return m_pluginLoader;
}





}
