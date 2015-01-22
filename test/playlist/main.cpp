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

using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    qmlRegisterUncreatableType<PhoenixPlayer::Common>("com.sunrain.playlist", 1, 0, "Common", "");

    Settings *settings = Settings::getInstance ();
    MusicLibrary::MusicLibraryManager *manager = MusicLibrary::MusicLibraryManager::getInstance ();
    manager->setSettings (settings);

    Common c;

//    //实现方式一
//    if (0) {
//        QQmlEngine engine;
//        QQmlContext *ctx = new QQmlContext(engine.rootContext ()); //view.rootContext();
//        ctx->setContextProperty ("musicLibraryManager", manager);
//        ctx->setContextProperty ("common", &c);

//        QQmlComponent component(&engine);
//        component.loadUrl (QUrl(QStringLiteral("qrc:/main.qml")));
//        component.create (ctx);

        QQmlApplicationEngine engine;
        QQmlContext *ctx = engine.rootContext ();
        ctx->setContextProperty ("musicLibraryManager", manager);
        ctx->setContextProperty ("common", &c);
        engine.load (QUrl(QStringLiteral("qrc:/main.qml")));

    return a.exec();
}
