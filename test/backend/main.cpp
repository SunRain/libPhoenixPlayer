#include <QCoreApplication>

#include <QDebug>

#include "BaseMediaObject.h"
#include "PlayBackendLoader.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PhoenixPlayer::Core::BaseMediaObject *obj = new PhoenixPlayer::Core::BaseMediaObject();
//    obj->setFileName("csyy.mp3");
//    obj->setFilePath("/media/wangguojian/MEDIA/music");
//    obj->setMediaType(PhoenixPlayerCore::Common::TypeLocalFile);

    QString url = "http://yinyueshiting.baidu.com/data2/music/127627006/127626827212400128.mp3?xcode=e589eec80fd7f64a9ab657ed87841175c737d0a3875e19c7&song_id=127626827";
    obj->setFilePath(url);
    obj->setMediaType(PhoenixPlayer::Common::TypeUrl);


    PhoenixPlayer::Core::PlayBackendLoader *loader = new PhoenixPlayer::Core::PlayBackendLoader();
    PhoenixPlayer::PlayBackend::IPlayBackend *backend = loader->getCurrentBackend();
    if (!backend) {
        qDebug() << "backend  is null";
    } else {
        backend->init();
        backend->changeMedia(obj, 0, true);

    }
    return a.exec();
}
