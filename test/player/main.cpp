#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCryptographicHash>

#include <QQmlEngine>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlContext>
#include <QScopedPointer>

#include <QDebug>
#include "Settings.h"
#include "MusicLibraryManager.h"
#include "PluginLoader.h"
#include "Player.h"
#include "LyricsModel.h"

#include "SingletonPointer.h"


using namespace PhoenixPlayer;
using namespace PhoenixPlayer::MusicLibrary;
using namespace PhoenixPlayer::PlayBackend;

int main(int argc, char *argv[])
{
//    QGuiApplication a(argc, argv);
    QScopedPointer<QGuiApplication> a(new QGuiApplication(argc, argv));
    qmlRegisterUncreatableType<Common>("com.sunrain.playlist", 1, 0, "Common", "");

    a.data ()->setOrganizationName ("SunRain");
    a.data ()->setApplicationName ("PhoenixPlayer");

    qmlRegisterType<QmlPlugin::LyricsModel>("com.sunrain.qmlplugin", 1, 0, "LyricsModel");

//    SingletonPointer<PluginLoader> p;
//    PluginLoader *loader = p.getInstance ();
    PluginLoader *loader = SingletonPointer<PluginLoader>::instance ();
    loader->setNewPlugin (PluginLoader::TypePlayBackend, "GStreamerBackend");
    loader->setParent (a.data ());

//    SingletonPointer<MusicLibraryManager> m;
//    MusicLibraryManager *manager = m.getInstance ();
    MusicLibraryManager *manager = SingletonPointer<MusicLibraryManager>::instance ();
    manager->setParent (a.data ());

    QScopedPointer<Player> musicPlayer(new Player(a.data ()));

    Common c;
    QQmlApplicationEngine engine;
    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("musicLibraryManager", manager);
    ctx->setContextProperty ("common", &c);
    ctx->setContextProperty ("musicPlayer", musicPlayer.data ());
    engine.load (QUrl(QStringLiteral("qrc:/main.qml")));

    return a.data ()->exec ();
}
