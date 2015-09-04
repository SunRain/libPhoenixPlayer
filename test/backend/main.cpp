#include <QCoreApplication>

#include <QDebug>
#include <QStandardPaths>

#include "Backend/BaseMediaObject.h"
#include "Backend/IPlayBackend.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "Player/VolumeControl.h"

using namespace PhoenixPlayer;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PhoenixPlayer::PlayBackend::BaseMediaObject *obj = new PhoenixPlayer::PlayBackend::BaseMediaObject();
//    obj->setFileName("csyy.mp3");
//    obj->setFilePath("/media/wangguojian/MEDIA/music");
//    obj->setMediaType(PhoenixPlayerCore::Common::TypeLocalFile);

//    QString url = "http://yinyueshiting.baidu.com/data2/music/127627006/127626827212400128.mp3?xcode=e589eec80fd7f64a9ab657ed87841175c737d0a3875e19c7&song_id=127626827";
    QString url = QString("%1/Music/test.mp3").arg (QStandardPaths::writableLocation (QStandardPaths::HomeLocation));
    qDebug()<<"url is "<<url;
    obj->setFilePath(url);
//    obj->setMediaType(PhoenixPlayer::Common::MediaTypeUrl);
    obj->setMediaType (PhoenixPlayer::Common::MediaTypeLocalFile);


//    PhoenixPlayer::PlayBackend::PlayBackendLoader *loader = new PhoenixPlayer::PlayBackend::PlayBackendLoader();
//    PhoenixPlayer::PlayBackend::IPlayBackend *backend = loader->getCurrentBackend();

//    SingletonPointer<PluginLoader> l;

//    PluginLoader *loader = l.getInstance ();
    PluginLoader *loader = PluginLoader::instance ();
    VolumeControl *v = VolumeControl::instance ();
    v->setMuted (false);
    v->setVolume (100, 100);
    PlayBackend::IPlayBackend *backend = loader->getCurrentPlayBackend ();
    if (!backend) {
        qDebug() << "backend  is null";
    } else {
        backend->init();
        backend->changeMedia(obj, 0, true);
    }
    return a.exec();
}
