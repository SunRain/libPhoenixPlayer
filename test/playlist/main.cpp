#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCryptographicHash>

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

    Settings *settings = Settings::getInstance ();
    PlayList::MusicLibraryManager *manager = PlayList::MusicLibraryManager::getInstance ();
    manager->setSettings (settings);

    manager->scanLocalMusic ();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return a.exec();
}
