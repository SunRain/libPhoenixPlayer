#include <QCoreApplication>
#include <QCryptographicHash>

#include <QDebug>
#include "DiskLookup.h"
#include "PlayListDAOLoader.h"
#include "Settings.h"
#include "MusicLibraryManager.h"

using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

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
//    QString str = "/home/wangguojian/Music/01 云儿.ape";
//    qDebug()<<str;

//    QCryptographicHash sha1(QCryptographicHash::Sha1);
//    QByteArray qba(str.toLatin1 ());
//    const char *c = qba.data ();
//    sha1.addData (c);
//    qDebug()<<sha1.result ().toHex ();
//    qDebug()<<QCryptographicHash::hash (str.toLocal8Bit (), QCryptographicHash::Sha1);
    return a.exec();
}
