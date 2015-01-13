#include <QCoreApplication>

#include <QDebug>
#include "DiskLookup.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PhoenixPlayer::PlayList::DiskLookup *lookup = PhoenixPlayer::PlayList::DiskLookup::getInstance ();
    lookup->startLookup ();
    return a.exec();
}
