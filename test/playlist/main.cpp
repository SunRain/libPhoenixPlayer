#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCryptographicHash>

#include <QQmlEngine>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlContext>

#include <QDebug>
#include "Settings.h"
#include "MusicLibraryManager.h"
#include "SingletonPointer.h"

using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    qmlRegisterUncreatableType<PhoenixPlayer::Common>("com.sunrain.playlist", 1, 0, "Common", "");

//    SingletonPointer<Settings> s;
//    Settings *settings = s.getInstance ();

    SingletonPointer<MusicLibrary::MusicLibraryManager> m;
    MusicLibrary::MusicLibraryManager *manager = m.getInstance ();
//    manager->initSettings (settings);

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
