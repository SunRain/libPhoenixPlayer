#include <QCoreApplication>

#include <QDebug>
#include "DiskLookup.h"
#include "PlayListDAOLoader.h"

using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PlayList::DiskLookup *lookup = PlayList::DiskLookup::getInstance ();
//    lookup->startLookup ();

    PlayList::PlayListDAOLoader *loader = PlayList::PlayListDAOLoader::getInstance ();
    PlayList::IPlayListDAO *dao = loader->getPlayListDAO ();

    if (!dao) {
        qDebug() <<" no dao found !!";
    }
    return a.exec();
}
