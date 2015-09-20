#ifndef IPLAYBACKEND_H
#define IPLAYBACKEND_H

#include <QObject>

#include "Common.h"
#include "BaseMediaObject.h"
#include "BaseVisual.h"

namespace PhoenixPlayer{
namespace PlayBackend {

class BaseVolume;
class IPlayBackend : public QObject {
    Q_OBJECT
public:
    explicit IPlayBackend(QObject *parent = 0) : QObject(parent) {
        m_visual = nullptr;
    }
    virtual ~IPlayBackend() {
        if (m_visual)
            m_visual = nullptr;
    }

    //    Q_INVOKABLE virtual void 	load_equalizer(vector<EQ_Setting>&)=0;
    virtual Common::PlayBackendState  playBackendState() = 0;
    virtual void init() = 0;
    // 返回不同playbackend的BaseVolume以供外部调用
    virtual BaseVolume *baseVolume() = 0;

    virtual bool useExternalDecoder() {
        return false;
    }
    virtual bool useExternalOutPut() {
        return false;
    }

    void setVisual(BaseVisual *v) {
        m_visual = v;
    }
protected:
    BaseVisual *getVisual() {
        return m_visual;
    }

signals:
//    void positionChanged(quint64 posMs = 0);
//    void volumeChanged(int vol = 0);
    //     void message(QMessageBox::Icon icon, const QString &title, const QString &msg) = 0;
    void mediaChanged(PlayBackend::BaseMediaObject *obj = 0);
    ///
    /// \brief finished 当前曲目结束
    ///
    void finished();
    ///
    /// \brief failed 当前曲目播放失败
    ///
    void failed();

    void stateChanged(Common::PlayBackendState state);
    void tick(quint64 sec = 0);

    //    void totalTimeChanged(qint64);
    //    void timeChangedSignal(quint32);
    //    void track_finished();
    //    void scrobble_track(const MetaData&);
    //    void eq_presets_loaded(const vector<EQ_Setting>&);
    //    void eq_found(const QStringList&);
    //    void sig_valid_strrec_track(const MetaData&);
    //    void wanna_gapless_track();
    //    void sig_level(float, float);

public slots:
    virtual void play(quint64 startSec = 0) = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
//    virtual void setVolume(int vol = 0) = 0;
    virtual void setPosition(quint64 sec = 0) = 0;
    virtual void changeMedia(PlayBackend::BaseMediaObject *obj = 0,
                             quint64 startSec = 0,
                             bool startPlay = false) = 0;
    //    virtual void jump(int where, bool percent = true) = 0;
    //    virtual void changeTrack(const SongMetaDate&, int pos_sec = 0, bool start_play = true) = 0;
    //    virtual void changeTrack(const QString&, int pos_sec = 0, bool start_play = true ) = 0;
    //    virtual void psl_gapless_track(const MetaData&) = 0;
    //    virtual void eq_changed(int, int) = 0;
    //    virtual void eq_enable(bool) = 0;
    //    virtual void record_button_toggled(bool) = 0;

    //    virtual void psl_sr_set_active(bool) = 0;
    //    virtual void psl_new_stream_session() = 0;
    //    virtual void psl_calc_level(bool) = 0;

    //private slots:
    //    virtual void sr_initialized(bool) = 0;
    //    virtual void sr_ended() = 0;
    //    virtual void sr_not_valid() = 0;
private:
    BaseVisual *m_visual;

};
} //namespace PlayBackend
} //namespace PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::PlayBackend::IPlayBackend, "PhoenixPlayer.PlayBackend.BasePlayBackendInterface/1.0")

#endif // IPLAYBACKEND_H
