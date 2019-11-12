#ifndef IPLAYBACKEND_H
#define IPLAYBACKEND_H

#include <QObject>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"
#include "BasePlugin.h"

namespace PhoenixPlayer{
    class MediaResource;

    namespace PlayBackend {
        class BaseVolume;

class LIBPHOENIXPLAYER_EXPORT IPlayBackend : public BasePlugin
{
    Q_OBJECT
public:
    explicit IPlayBackend(QObject *parent = Q_NULLPTR)
        : BasePlugin(parent)
    {

    }
    virtual ~IPlayBackend() override
    {

    }

    virtual PPCommon::PlayBackendState  playBackendState() = 0;

    virtual void initialize() = 0;

    // 返回不同playbackend的BaseVolume以供外部调用
    virtual BaseVolume *baseVolume() = 0;

//    virtual bool useExternalDecoder()
//    {
//        return false;
//    }

//    virtual bool useExternalOutPut()
//    {
//        return false;
//    }


    ///
    /// \brief durationInMS
    /// \return <=0 if not find duration
    ///
    virtual qint64 durationInMS() {
        return -1;
    }

    // BasePlugin interface
public:
    virtual PluginProperty property() const Q_DECL_OVERRIDE
    {
        return PluginProperty();
    }
    virtual PluginType type() const Q_DECL_OVERRIDE
    {
        return BasePlugin::PluginPlayBackend;
    }
    virtual QWidget *settingsUI() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }
    virtual QWidget *aboutUI() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
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

#endif // IPLAYBACKEND_H
