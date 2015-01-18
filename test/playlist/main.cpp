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

//    PlayList::DiskLookup *lookup = PlayList::DiskLookup::getInstance ();
////    lookup->startLookup ();

//    PlayList::PlayListDAOLoader *loader = PlayList::PlayListDAOLoader::getInstance ();
//    PlayList::IPlayListDAO *dao = loader->getPlayListDAO ();

//    if (!dao) {
//        qDebug() <<" no dao found !!";
//    }
//    dao->initDataBase ();
    qmlRegisterUncreatableType<PhoenixPlayer::Common>("com.sunrain.playlist", 1, 0, "Common", "");

    Settings *settings = Settings::getInstance ();
    PlayList::MusicLibraryManager *manager = PlayList::MusicLibraryManager::getInstance ();
    manager->setSettings (settings);

    Common c;
//    manager->scanLocalMusic ();

//    QQmlApplicationEngine engine;
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QQuickView view;
    view.setResizeMode (QQuickView::SizeRootObjectToView);
    QQmlContext *ctxt = view.rootContext();
    ctxt->setContextProperty ("musicLibraryManager", manager);
    ctxt->setContextProperty ("common", &c);

    view.setSource (QUrl(QStringLiteral("qrc:/main.qml")));
    view.show ();

    return a.exec();
}
