#include "LibPhoenixPlayerMain.h"

#include <QtQml>
#include <QDebug>

#include "Settings.h"
#include "PluginLoader.h"
#include "MusicLibrary/MusicLibraryManager.h"
#include "PlayerCore/VolumeControl.h"
#include "PlayerCore/PlayerCore.h"
#include "MusicLibrary/LocalMusicScanner.h"

#include "Common.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;
using namespace MetadataLookup;

LibPhoenixPlayer::LibPhoenixPlayer(QObject *parent)
    : QObject(parent)
//    , m_initiated(false)
//    , m_settings(nullptr)
//    , m_libraryMgr(nullptr)
//    , m_volumeCtrl(nullptr)
//    , m_playerCore(nullptr)
//    , m_lookupMgr(nullptr)
//    , m_pluginLoader(nullptr)
//    , m_localScanner(nullptr)
{
    qDebug()<<">>>>>> aaaaa <<<<";
    m_settings = new Settings();
    qDebug()<<">>>>>> bbbb <<<<";
    m_pluginLoader = new PluginLoader(m_settings);
    qDebug()<<">>>>>> ccccc <<<<";
    m_libraryMgr = new MusicLibraryManager(m_settings, m_pluginLoader);
    qDebug()<<">>>>>> ddddd <<<<";
    m_volumeCtrl = new VolumeControl(m_pluginLoader);
     qDebug()<<">>>>>> eeeee <<<<";
    m_playerCore = new PlayerCore(m_settings, m_pluginLoader, m_libraryMgr);
    qDebug()<<">>>>>> fffff <<<<";
    m_initiated = true;
}

LibPhoenixPlayer::~LibPhoenixPlayer()
{

}

void LibPhoenixPlayer::initiate()
{
//    if (m_initiated)
//        return;
//    m_settings = new Settings();
//    m_pluginLoader = new PluginLoader(m_settings);
//    m_libraryMgr = new MusicLibraryManager(m_settings, m_pluginLoader);
//    m_volumeCtrl = new VolumeControl(m_pluginLoader);
//    m_playerCore = new PlayerCore(m_settings, m_pluginLoader, m_libraryMgr);
////    m_localScanner = new LocalMusicScanner(m_settings);
//    m_initiated = true;
    qDebug()<<">>>>>>>>>>>>>>>>>>>>>>"<<Q_FUNC_INFO<<"<<<<<<<<<<<<<";
    m_playerCore->init ();
}

void LibPhoenixPlayer::registerPlugins(const char *url)
{
    checkInitiate ();

    qDebug()<<Q_FUNC_INFO<<" url "<<url;

    qmlRegisterUncreatableType<Common>(url, 1, 0, "Common", "Cannot be created");
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
