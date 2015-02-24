#ifndef GSTREAMERBACKEND_H
#define GSTREAMERBACKEND_H

#include <QStringList>

#include <gst/gst.h>
#include <gst/gstbuffer.h>

#ifdef SAILFISH_OS
#include <AudioResourceQt>
#endif

#include "Backend/IPlayBackend.h"
#include "Backend/BaseMediaObject.h"

class QTimer;
namespace PhoenixPlayer {
namespace PlayBackend {
namespace GStreamer {

#define CAPS_TYPE_INT 0
#define CAPS_TYPE_FLOAT 1
#define CAPS_TYPE_UNKNOWN -1


#define STATE_STOP 0
#define STATE_PLAY 1
#define STATE_PAUSE 2

#define EQ_TYPE_NONE -1
#define EQ_TYPE_KEQ 0
#define EQ_TYPE_10B 1

#define BACKEND_NAME "GStreamerBackend"
#define BACKEND_VERSION "0.1"
#define DESCRIPTION "Gstreamer play backend"

//float f_channel[2];
#define SCALE_SHORT 0.000000004f
#define BUFFER_SIZE 4096

struct MyCaps {
    int type;
    bool sig;
    int width;
    int channels;
    bool is_parsed;

    void parse(GstCaps* caps){
        QString info = gst_caps_to_string(caps);
        //qDebug() << info;

        QStringList lst = info.split(",");
        foreach(QString s, lst){
            s = s.trimmed();
            if(s.startsWith("audio", Qt::CaseInsensitive)){
                if(s.contains("int", Qt::CaseInsensitive)) type = CAPS_TYPE_INT;
                else if(s.contains("float", Qt::CaseInsensitive)) type = CAPS_TYPE_FLOAT;
                else type = CAPS_TYPE_UNKNOWN;
            }
            else if(s.startsWith("signed", Qt::CaseInsensitive)){
                if(s.contains("true", Qt::CaseInsensitive)) sig = true;
                else sig = false;
            }
            else if(s.startsWith("width", Qt::CaseInsensitive)){
                width = s.right(2).toInt();
            }
            else if(s.startsWith("channels", Qt::CaseInsensitive)){
                channels = s.right(1).toInt();
                if(channels > 2) channels = 2;
            }
        }
        is_parsed = true;
    }
};

class Common;
class GStreamerBackend : public IPlayBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayBackend.GStreamerBackend" FILE "gstreamerbackend.json")
    Q_INTERFACES(PhoenixPlayer::PlayBackend::IPlayBackend)
public:
    GStreamerBackend (QObject *parent = 0);
    virtual ~GStreamerBackend();

// IPlayBackend interface
    PhoenixPlayer::Common::PlaybackState getPlaybackState();
    QString getBackendName();
    QString getBackendVersion();
    QString getDescription();
    void init();


    void state_changed();
    void set_cur_position(quint32 pos_sec = 0);
    void set_track_finished();
    void set_about_to_finish();
    void set_buffer(GstBuffer *buffer = 0);
    void emit_buffer(float inv_arr_channel_elements, float scale);
//    virtual int		getState();

public slots:
    // IPlayBackend interface
    void play(quint64 startSec = 0);
    void stop();
    void pause();
    void setVolume(int vol = 0);
    void setPosition(quint64 sec = 0);
    void changeMedia(PlayBackend::BaseMediaObject *obj = 0,
                     quint64 startSec = 0,
                     bool startPlay = false);

private:
    void init_play_pipeline();
    bool set_uri(PlayBackend::BaseMediaObject *obj = 0, bool startPlay = false);
    void gstPlay(quint64 startSec = 0);
    void gstPause();
    void gstStop();
private:
    GstElement* _pipeline;
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

//    CSettingsStorage* _settings;

//    StreamRecorder* _stream_recorder;

//    LastTrack*  _last_track;
    MyCaps     _caps;


    PhoenixPlayer::Common::PlaybackState _state;
    bool		_is_eq_enabled;
    int			_eq_type;
    bool		_track_finished;

    int         _vol;
    bool _playing_stream;
     bool        _gapless_track_available;
     bool __start_pos_beginning;
     bool __start_at_beginning;
     int			_seconds_started;
     int			_seconds_now;

     bool		_scrobbled;

#ifdef SAILFISH_OS
     AudioResourceQt::AudioResource mAudioResource;
     quint64 mStartSec;
     QTimer *mTimer;
#endif
};
} //GStreamer
} //PlayBackend
} //PhoenixPlayer
#endif // GSTREAMERBACKEND_H
