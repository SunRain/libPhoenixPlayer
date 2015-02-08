#include <QCoreApplication>

#include <QDebug>

#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"

using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PhoenixPlayer::PlayBackend::BaseMediaObject *obj = new PhoenixPlayer::PlayBackend::BaseMediaObject();
//    obj->setFileName("csyy.mp3");
//    obj->setFilePath("/media/wangguojian/MEDIA/music");
//    obj->setMediaType(PhoenixPlayerCore::Common::TypeLocalFile);

    QString url = "http://yinyueshiting.baidu.com/data2/music/127627006/127626827212400128.mp3?xcode=e589eec80fd7f64a9ab657ed87841175c737d0a3875e19c7&song_id=127626827";
    obj->setFilePath(url);
    obj->setMediaType(PhoenixPlayer::Common::MediaTypeUrl);


//    PhoenixPlayer::PlayBackend::PlayBackendLoader *loader = new PhoenixPlayer::PlayBackend::PlayBackendLoader();
//    PhoenixPlayer::PlayBackend::IPlayBackend *backend = loader->getCurrentBackend();

//    SingletonPointer<PluginLoader> l;

//    PluginLoader *loader = l.getInstance ();
    PluginLoader *loader = SingletonPointer<PluginLoader>::instance ();
    PlayBackend::IPlayBackend *backend = loader->getCurrentPlayBackend ();
    if (!backend) {
        qDebug() << "backend  is null";
    } else {
        backend->init();
        backend->changeMedia(obj, 0, true);

    }
    return a.exec();
}
