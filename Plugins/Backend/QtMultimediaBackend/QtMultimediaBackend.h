#ifndef QTMULTIMEDIABACKEND_H
#define QTMULTIMEDIABACKEND_H

#include "Backend/IPlayBackend.h"
#include "Backend/BaseMediaObject.h"

class QMediaPlayer;
namespace PhoenixPlayer {
namespace PlayBackend {
namespace QtMultimediaBackend {

#define BACKEND_NAME "QtMultimediaBackend"
#define BACKEND_VERSION "0.1"
#define DESCRIPTION "QtMultimedia play backend"

class QtMultimediaBackend : public IPlayBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayBackend.QtMultimediaBackend" FILE "qmultimediabackend.json")
    Q_INTERFACES(PhoenixPlayer::PlayBackend::IPlayBackend)
public:
    QtMultimediaBackend(QObject *parent = 0);
    virtual ~QtMultimediaBackend();

    // IPlayBackend interface
public:
    Common::PlayBackendState getPlayBackendState();
    QString getBackendName();
    QString getBackendVersion();
    QString getDescription();
    void init();

public slots:
    void play(quint64 startSec = 0);
    void stop();
    void pause();
    void setVolume(int vol = 0);
    void setPosition(quint64 sec = 0);
    void changeMedia(BaseMediaObject *obj = 0, quint64 startSec = 0, bool startPlay = false);

//signals:

//    void mediaChanged(PlayBackend::BaseMediaObject *obj = 0);
//    ///


private:
    PhoenixPlayer::Common::PlayBackendState mPlayBackendState;
    QMediaPlayer *mPlayer;
};

} //QtMultimediaBackend
} //PlayBackend
} //PhoenixPlayer
#endif // QTMULTIMEDIABACKEND_H
