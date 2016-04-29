#ifndef PHOENIXPLAYBACKEND_H
#define PHOENIXPLAYBACKEND_H

#include "Backend/IPlayBackend.h"

namespace PhoenixPlayer {
class VolumeControl;
namespace PlayBackend {
namespace PhoenixBackend {

class PlayThread;
class StateHandler;
class PhoenixPlayBackend : public IPlayBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayBackend.PhoenixPlayBackend" FILE "PhoenixBackend.json")
    Q_INTERFACES(PhoenixPlayer::PlayBackend::IPlayBackend)
public:
    explicit PhoenixPlayBackend(QObject *parent = 0);
    virtual ~PhoenixPlayBackend();

    // QObject interface
public:
    bool event(QEvent *e);

    // IPlayBackend interface
public:
    Common::PlayBackendState playBackendState();
    void initialize();
    BaseVolume *baseVolume();
    bool useExternalDecoder();
    bool useExternalOutPut();

public slots:
    void play(quint64 startSec = 0);
    void stop();
    void pause();
//    void setVolume(int vol);
    void setPosition(quint64 sec = 0);
    void changeMedia(BaseMediaObject *obj, quint64 startSec = 0, bool startPlay = false);

signals:
//    void volumeChanged(int vol = 0);
//    //     void message(QMessageBox::Icon icon, const QString &title, const QString &msg) = 0;
//    void mediaChanged(PlayBackend::BaseMediaObject *obj = 0);
//    ///
//    /// \brief finished 当前曲目结束
//    ///
//    void finished();
//    ///
//    /// \brief failed 当前曲目播放失败
//    ///
//    void failed();

//    void stateChanged(Common::PlayBackendState state);
//    void tick(quint64 sec = 0);
public slots:

private:
    PlayThread *m_engine;
    StateHandler *m_handler;
    VolumeControl *m_volumeControl;

    QString m_url;

    int m_nextState;
//    quint64 m_preSec;
    bool m_muted;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // PHOENIXPLAYBACKEND_H
