#ifndef PHOENIXPLAYBACKEND_H
#define PHOENIXPLAYBACKEND_H

#include "Backend/IPlayBackend.h"

namespace PhoenixPlayer {
    class VolumeControl;
    class MediaResource;

    namespace PlayBackend {

        namespace PhoenixBackend {
            class DecodeThread;
            class OutputThread;
            class StateHandler;
            class AudioEffect;
            class Recycler;

class PhoenixPlayBackend : public IPlayBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayBackend.PhoenixPlayBackend" FILE "PhoenixBackend.json")
    Q_INTERFACES(PhoenixPlayer::PlayBackend::IPlayBackend)
public:
    explicit PhoenixPlayBackend(QObject *parent = Q_NULLPTR);
    virtual ~PhoenixPlayBackend() override;

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
    qint64 durationInMS() Q_DECL_OVERRIDE;

public slots:
    void play(quint64 startSec = 0) Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void pause() Q_DECL_OVERRIDE;
    void setPosition(quint64 sec) Q_DECL_OVERRIDE;
    void changeMedia(MediaResource *res, quint64 startSec, bool startPlay) Q_DECL_OVERRIDE;



private:
    BaseVolume      *m_volume           = Q_NULLPTR;
    StateHandler    *m_handler          = Q_NULLPTR;
    VolumeControl   *m_volumeControl    = Q_NULLPTR;
    DecodeThread      *m_playThread       = Q_NULLPTR;
    OutputThread    *m_outputThread     = Q_NULLPTR;
    MediaResource   *m_resource         = Q_NULLPTR;
    Recycler        *m_recycler         = Q_NULLPTR;
    bool            m_muted = false;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // PHOENIXPLAYBACKEND_H
