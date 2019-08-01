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
    explicit PhoenixPlayBackend(QObject *parent = Q_NULLPTR);
    virtual ~PhoenixPlayBackend();

    // QObject interface
public:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

    // IPlayBackend interface
public:
    PPCommon::PlayBackendState playBackendState() Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;
    BaseVolume *baseVolume() Q_DECL_OVERRIDE;
    bool useExternalDecoder() Q_DECL_OVERRIDE;
    bool useExternalOutPut() Q_DECL_OVERRIDE;

public slots:
    void play(quint64 startSec) Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void pause() Q_DECL_OVERRIDE;
    void setPosition(quint64 sec) Q_DECL_OVERRIDE;
    void changeMedia(MediaResource *res, quint64 startSec, bool startPlay) Q_DECL_OVERRIDE;

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
