#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCryptographicHash>

#include <QQmlEngine>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlContext>

#include <QDebug>
#include "DiskLookup.h"
#include "PlayListDAOLoader.h"
#include "Settings.h"
#include "MusicLibraryManager.h"
#include "PlayBackendLoader.h"
#include "Player.h"


using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    qmlRegisterUncreatableType<PhoenixPlayer::Common>("com.sunrain.playlist", 1, 0, "Common", "");

    Settings *settings = Settings::getInstance ();
    MusicLibrary::MusicLibraryManager *manager = MusicLibrary::MusicLibraryManager::getInstance ();
    manager->setSettings (settings);

    PlayBackend::PlayBackendLoader *backendLoader = PlayBackend::PlayBackendLoader::getInstance ();
    backendLoader->setNewBackend ("GStreamerBackend");

    Player *musicPlayer = new Player();
    musicPlayer->setSettings (settings);
    musicPlayer->setPlayBackendLoader (backendLoader);
    musicPlayer->setMusicLibraryManager (manager);

    Common c;
    QQmlApplicationEngine engine;
    QQmlContext *ctx = engine.rootContext ();
    ctx->setContextProperty ("musicLibraryManager", manager);
    ctx->setContextProperty ("common", &c);
    ctx->setContextProperty ("musicPlayer", musicPlayer);
    engine.load (QUrl(QStringLiteral("qrc:/main.qml")));

    return a.exec();
}
