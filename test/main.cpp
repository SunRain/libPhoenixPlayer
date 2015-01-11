#include <QCoreApplication>

#include <QDebug>

#include "BaseMediaObject.h"
#include "PlayBackendLoader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PhoenixPlayerCore::BaseMediaObject *obj = new PhoenixPlayerCore::BaseMediaObject();
    obj->setFileName("Lydia.mp3");
    obj->setFilePath("/media/wangguojian/MEDIA/music/");
    obj->setMediaType(PhoenixPlayerCore::Common::TypeLocalFile);

    PhoenixPlayerCore::PlayBackendLoader *loader = new PhoenixPlayerCore::PlayBackendLoader();
    PhoenixPlayerCore::IPlayBackend *backend = loader->getCurrentBackend();
    if (!backend) {
        qDebug() << "backend  is null";
    } else {
        backend->init();
        backend->changeMedia(obj, 0, true);

    }
    return a.exec();
}
