#ifndef GSTREAMER_H
#define GSTREAMER_H

#include <QTimer>

#include <gst/gst.h>

#include "IPlayBackend.h"
#include "BaseMediaObject.h"

namespace PhoenixPlayerCore {

class Gstreamer : public IPlayBackend, public QObject
{
    Q_OBJECT
    Q_INTERFACES(IPlayBackend)
public:
    Gstreamer(QObject *parent = 0);
    virtual ~Gstreamer();

    // IPlayBackend interface
public:
    Common::PlaybackState getPlaybackState();
    QString getBackendName();
    QString getBackendVersion();
    void init();

    void 		set_buffer(GstBuffer*);

    void _finish();
    void _emitError(QString error);
    void _fail();

public slots:
    void play(quint64 startMs);
    void stop();
    void pause();
    void setVolume(int vol);
    void setPosition(quint64 posMs = 0);
    void changeMedia(BaseMediaObject *obj = 0, quint64 startMs = 0, bool startPlay = false);
private slots:
    void checkStatus();

private:
    void init_play_pipeline();
     bool set_uri(BaseMediaObject *obj = 0, const bool &startPlay = 0);


private:
    QTimer *m_timer;
    GstElement *m_playbin;
    GstElement* _equalizer;
    GstElement* _eq_queue;
    GstElement* _volume;

    GstPad* _tee_app_pad;
    GstPad* _app_pad;

    GstElement* _audio_sink;
    GstElement* _audio_bin;
    GstElement* _gio_src;
    GstElement* _decodebin;

    GstElement* _app_sink;
    GstElement* _app_queue;

    GstElement* _tee;

    GstBus*		_bus;

    int         _vol;
    bool _playing_stream;
     bool		_track_finished;
     bool        _gapless_track_available;

     Common::PlaybackState _state;
};

}
#endif // GSTREAMER_H
