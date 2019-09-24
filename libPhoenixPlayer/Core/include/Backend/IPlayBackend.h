#ifndef IPLAYBACKEND_H
#define IPLAYBACKEND_H

#include <QObject>

#include "PPCommon.h"
#include "libphoenixplayer_global.h"

namespace PhoenixPlayer{
    class MediaResource;

    namespace PlayBackend {
        class BaseVolume;

class LIBPHOENIXPLAYER_EXPORT IPlayBackend : public QObject {
    Q_OBJECT
public:
    explicit IPlayBackend(QObject *parent = Q_NULLPTR): QObject(parent) {
    }
    virtual ~IPlayBackend() {
    }

    virtual PPCommon::PlayBackendState  playBackendState() = 0;

    virtual void initialize() = 0;

    // 返回不同playbackend的BaseVolume以供外部调用
    virtual BaseVolume *baseVolume() = 0;

    virtual bool useExternalDecoder() {
        return false;
    }

    virtual bool useExternalOutPut() {
        return false;
    }


    ///
    /// \brief durationInMS
    /// \return <=0 if not find duration
    ///
    virtual qint64 durationInMS() {
        return -1;
    }

signals:
    void resourceChanged(MediaResource *res);
    ///
    /// \brief finished 当前曲目结束
    ///
    void finished();
    ///
    /// \brief failed 当前曲目播放失败
    ///
    void failed();

    void stateChanged(PPCommon::PlayBackendState state);

    Q_DECL_DEPRECATED void tick(quint64 sec = 0);
    void tickInMS(quint64 ms);
    void tickInSec(quint64 sec);

public slots:
    virtual void play(quint64 startSec = 0) = 0;

    virtual void stop() = 0;

    virtual void pause() = 0;

    //TODO use milliseconds
    virtual void setPosition(quint64 sec = 0) = 0;

    virtual void changeMedia(MediaResource *res,
                             quint64 startSec = 0,
                             bool startPlay = false) = 0;

};
} //namespace PlayBackend
} //namespace PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::PlayBackend::IPlayBackend, "PhoenixPlayer.PlayBackend.BasePlayBackendInterface/1.0")

#endif // IPLAYBACKEND_H
