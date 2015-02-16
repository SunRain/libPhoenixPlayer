#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>

#include <glib/gconvert.h>
#include <glib/gmain.h>
#include <glib-unix.h>

//#include <QtPlugin>

#include "GStreamerBackend.h"
#include "Common.h"
#include "GStreamerBackendUtil.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace GStreamer {

float f_channel[2];

void _calc_log10_lut(){
    for(int i=0; i<=20000; i++){
        log_10[i] = log10(i / 20000.0f);
    }
    for(int i=0; i<128; i++){
        lo_128[i] = i*128.0f;
    }
}

GStreamerBackend::GStreamerBackend(QObject *parent)
    :IPlayBackend(parent)
{
    qDebug()<<"GStreamerBackend >>>>>>>>"<<__FUNCTION__;

    _caps.is_parsed = false;
    _calc_log10_lut();
//    _settings = CSettingsStorage::getInstance();
//    _name = "GStreamer Backend";
    _state = PhoenixPlayer::Common::PlaybackStopped; //STATE_STOP;

    _seconds_started = 0;
    _seconds_now = 0;
    _scrobbled = false;
    _track_finished = true;

    _playing_stream = false;
//    _sr_active = false;
//    _sr_wanna_record = false;

    _gapless_track_available = false;


    _pipeline = nullptr;
    _equalizer = nullptr;
    _eq_queue = nullptr;
    _volume = nullptr;

    _tee_app_pad  = nullptr;
    _app_pad = nullptr;

    _audio_sink = nullptr;
    _audio_bin = nullptr;
    _gio_src = nullptr;
    _decodebin = nullptr;

    _app_sink = nullptr;
    _app_queue = nullptr;

    _tee = nullptr;

    _bus = nullptr;
//    _stream_recorder = new StreamRecorder();

      /* _timer = new QTimer(this);
       _timer->setInterval(10);

       connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));*/


//       connect(_stream_recorder, SIGNAL(sig_initialized(bool)), this, SLOT(sr_initialized(bool)));
//       connect(_stream_recorder, SIGNAL(sig_stream_ended()), this,
//               SLOT(sr_ended()));
//       connect(_stream_recorder, SIGNAL(sig_stream_not_valid()), this,
//               SLOT(sr_not_valid()));
}

GStreamerBackend::~GStreamerBackend()
{
    qDebug() << "Engine: close engine... ";

//    _settings->updateLastTrack();
    if (_bus)
        gst_object_unref (_bus);
    if (_pipeline) {
        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
        gst_object_unref (GST_OBJECT(_pipeline));
    }
    gst_obj_ref = nullptr;
}

PhoenixPlayer::Common::PlaybackState GStreamerBackend::getPlaybackState()
{
    return _state;
}

QString GStreamerBackend::getBackendName()
{
    return BACKEND_NAME;
}

QString GStreamerBackend::getBackendVersion()
{
    return BACKEND_VERSION;
}

QString GStreamerBackend::getDescription()
{
    return QString(DESCRIPTION);
}

void GStreamerBackend::init()
{
    gst_init(0, 0);
    init_play_pipeline();
}

void GStreamerBackend::state_changed()
{

}

void GStreamerBackend::set_cur_position(quint32 pos_sec)
{
    if ((quint32) _seconds_now == pos_sec)
           return;
       _seconds_now = pos_sec;
//       int playtime = _seconds_now - _seconds_started;

//       if (!_scrobbled
//               && (playtime >= 15 || playtime == _meta_data.length_ms / 2000)) {

//           emit scrobble_track(_meta_data);
//           _scrobbled = true;
//       }

//       _last_track->id = _meta_data.id;
//       _last_track->filepath = _meta_data.filepath;
//       _last_track->pos_sec = pos_sec;

//       qDebug()<<"set_cur_position positionChanged to"<<_seconds_now;

       //emit positionChanged(_seconds_now);
       emit tick (pos_sec);
}

void GStreamerBackend::set_track_finished()
{
//    if (_sr_active && _playing_stream) {
//        _stream_recorder->stop(!_sr_wanna_record);
//    }

//    emit track_finished();
    emit finished();
    _track_finished = true;
}

void GStreamerBackend::set_about_to_finish()
{
    qDebug() << "Engine: about to finish " << _gapless_track_available;
//      if (_gapless_track_available && 0) {

//          bool b = true;
//          set_uri(_md_gapless, b);
//          _gapless_track_available = false;
//          _track_finished = true;
//          emit track_finished();
    //      }
}

void GStreamerBackend::set_buffer(GstBuffer *buffer)
{
    if(!_caps.is_parsed){

        GstCaps* caps = gst_buffer_get_caps(buffer);
        _caps.parse(caps);
    }

    f_channel[0] = 0;
    f_channel[1] = 0;

    //guint64 dur = GST_BUFFER_DURATION(buffer);
    gsize sz = GST_BUFFER_SIZE(buffer);
    guint8* c_buf = GST_BUFFER_DATA(buffer);
    float scale = 1.0f;



    // size of one element in bytes
    int item_size = _caps.width / 8;

    // array has sz bytes, but only sz / el_size elements
    // and every channel has it
    gsize end = sz - item_size;
    gsize start = 0;
    gsize thousand_samples = item_size * _caps.channels * 512;
    if(sz > thousand_samples) start = sz - thousand_samples;
    float inv_arr_channel_elements = (item_size * _caps.channels * 1.0) / (sz -start);



    int channel=0;

    if(_caps.type == CAPS_TYPE_FLOAT){

        float *v_f;

        for(gsize i=start; i<end; i+=item_size){

            v_f = (float*) (c_buf+i);

            f_channel[channel] += ( (*v_f) * (*v_f));
            channel = (channel + 1) % _caps.channels;
        }
    }

    else if(_caps.type == CAPS_TYPE_INT){
        short* v_s;
        float v;
        scale = SCALE_SHORT;

        for(gsize i=start; i<end; i+=item_size){

            v_s = (short*) (c_buf + i);
            v = (float) (*v_s);
            f_channel[channel] += (v * v);
            channel = (channel + 1) % _caps.channels;
        }
    }
    qDebug()<<" inv_arr_channel_elements " << inv_arr_channel_elements
           <<" scale "<<scale;

    emit_buffer(inv_arr_channel_elements, scale);

    gst_buffer_unref(buffer);
}

void GStreamerBackend::emit_buffer(float inv_arr_channel_elements, float scale)
{
    for(int i=0; i<_caps.channels; i++){
        float val = f_channel[i] * inv_arr_channel_elements * scale;
        if(val > 1.0f) val = 1.0f;
        f_channel[i] = 10.0f * LOOKUP_LOG(val);
    }

//    if(_caps.channels >= 2)
//        emit sig_level(f_channel[0], f_channel[1]);

//    else if(_caps.channels == 1)
//        emit sig_level(f_channel[0], -100.0f);

    f_channel[0] = 0;
    f_channel[1] = 0;
}

//int GStreamerBackend::getState()
//{
//    return _state;
//}

void GStreamerBackend::play(quint64 startSec)
{
    _track_finished = false;
    _state = PhoenixPlayer::Common::PlaybackPlaying;
    emit stateChanged(_state);

    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

    gst_obj_ref = this;
    if (startSec > 0)
        setPosition(startSec);
}

void GStreamerBackend::stop()
{
    _state = PhoenixPlayer::Common::PlaybackStopped;
    emit stateChanged(_state);

        // streamripper, wanna record is set when record button is pressed
//        if (_playing_stream && _sr_active) {
//            qDebug() << "Engine: stop... Playing stream";
//            _stream_recorder->stop(!_sr_wanna_record);
//        }

        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);

        _track_finished = true;
}

void GStreamerBackend::pause()
{
    _state = PhoenixPlayer::Common::PlaybackPaused;
    emit stateChanged(_state);
    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}

void GStreamerBackend::setVolume(int vol)
{
    _vol = vol;

    float vol_val = (float) (vol * 1.0f / 100.0f);

    g_object_set(G_OBJECT(_pipeline), "volume", vol_val, NULL);

    emit volumeChanged(_vol);
}

void GStreamerBackend::setPosition(quint64 sec)
{
    //GStreamer used nanosecond
    gint64 newTimeNs = sec * GST_SECOND;
    if (!gst_element_seek_simple(_pipeline, GST_FORMAT_TIME,
                                 (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
                                 newTimeNs)) {
        qDebug() << "seeking failed ";
    }
    emit tick (sec);
}

void GStreamerBackend::changeMedia(PlayBackend::BaseMediaObject *obj,
                                                      quint64 startSec,
                                                      bool startPlay)
{
    qDebug()<<"GStreamerBackend >>>>>>>>"<<__FUNCTION__;

    _caps.is_parsed = false;

    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
    gst_obj_ref = this;

    //    _last_track = _settings->getLastTrack();

    if (!_track_finished /*|| (md.filepath != _md_gapless.filepath)*/ || true) {
        stop();
        //        _meta_data = md;
        // when stream ripper, do not start playing
        _playing_stream = (obj->mediaType() == PhoenixPlayer::Common::MediaTypeStream
                           || obj->mediaType() == PhoenixPlayer::Common::MediaTypeUrl);/*Helper::is_www(md.filepath);*/

        qDebug()<<"=== _playing_stream " << _playing_stream;

        bool success = set_uri(obj, startPlay);
        if (!success)
            return;

        g_timeout_add(500, (GSourceFunc) show_position, _pipeline);

        _gapless_track_available = false;
        //emit wanna_gapless_track();
        ///TODO: 添加信号
        //        emit total_time_changed_signal(_meta_data.length_ms);
        //        emit timeChangedSignal(startSec);
        //emit positionChanged(startMs);
        emit tick (startSec);
    } else {
        //        _meta_data = _md_gapless;
        _gapless_track_available = false;

        ///TODO: 添加信号
        //        emit wanna_gapless_track();
        //        emit total_time_changed_signal(_meta_data.length_ms);

        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
    }

    _seconds_started = 0;
    _seconds_now = 0;
    _scrobbled = false;
    _track_finished = false;

    if (startSec > 0) {
        __start_pos_beginning = startSec;
        __start_at_beginning = false;
    }

    else {
        __start_at_beginning = true;
    }

    qDebug()<<" __start_at_beginning "<<__start_at_beginning;

    if (startPlay) {
        play(startSec);
    } else if (!startPlay /*&& !(_playing_stream && _sr_active)*/) { // pause if streamripper is not active
        pause();
    }

}

void GStreamerBackend::init_play_pipeline()
{
    qDebug()<<"GStreamerBackend >>>>>>>>"<<__FUNCTION__;

    bool success = false;
    bool with_app_sink = true;
    int i = 0;

    // eq -> autoaudiosink is packaged into a bin
    do {
        // create equalizer element
        _pipeline = gst_element_factory_make("playbin2", "player");
        _test_and_error(_pipeline, "Engine: Pipeline sucks");


        _bus = gst_pipeline_get_bus(GST_PIPELINE(_pipeline));
        _audio_bin = gst_bin_new("audio-bin");
        _equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");


        _decodebin = gst_element_factory_make("decodebin2", "decoder");
        _audio_sink = gst_element_factory_make("autoaudiosink", "autoaudiosink");

        _eq_queue = gst_element_factory_make("queue", "eq_queue");
        _tee = gst_element_factory_make("tee", "tee");
        _app_queue = gst_element_factory_make("queue", "app_queue");
        _app_sink = gst_element_factory_make("appsink", "app_sink");


        if(!_test_and_error(_bus, "Engine: Something went wrong with the bus")) break;
        //if(!_test_and_error(_gio_src, "Engine: Giosrc fail")) break;
        if(!_test_and_error(_decodebin, "Engine: decodebin fail")) break;
        if(!_test_and_error(_audio_bin, "Engine: Bin cannot be created")) break;
        if(!_test_and_error(_tee, "Engine: Tee cannot be created")) break;
        if(!_test_and_error(_equalizer, "Engine: Equalizer cannot be created")) break;
        if(!_test_and_error(_eq_queue, "Engine: Equalizer cannot be created")) break;
        if(!_test_and_error(_audio_sink, "Engine: Audio Sink cannot be created")) break;
        if(!_test_and_error(_app_queue, "Engine: Queue cannot be created")) break;
        if(!_test_and_error(_app_sink, "Engine: App Sink cannot be created")) break;

        gst_bus_add_watch(_bus, bus_state_changed, this);

        // create a bin that includes an equalizer and replace the sink with this bin

        //if(!_test_and_error_bool(success, "Engine: Cannot link src with decoder")) break;

        if(with_app_sink){
            gst_bin_add_many(GST_BIN(_audio_bin), _tee, _eq_queue, _equalizer, _audio_sink, _app_queue, _app_sink, NULL);
            success = gst_element_link_many(_app_queue, _app_sink, NULL);
            _test_and_error_bool(success, "Engine: Cannot link queue with app sink");
            success = gst_element_link_many(_eq_queue, _equalizer, _audio_sink, NULL);
        }

        if(!with_app_sink || !success){
            with_app_sink = false;
            gst_bin_add_many(GST_BIN(_audio_bin), _equalizer, _audio_sink, NULL);
            success = gst_element_link_many(_equalizer, _audio_sink, NULL);
        }

        if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) break;

        // Connect tee
        GstPadTemplate* tee_src_pad_template;
        GstPad* tee_pad;
        GstPad* tee_eq_pad;
        GstPad* tee_app_pad;
        GstPad* eq_pad;
        GstPad* app_pad;
        GstPadLinkReturn s;

        if(with_app_sink){
            // create tee pads
            tee_src_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(_tee), "src%d");

            // "outputs" of tee to eq and queue

            tee_eq_pad = gst_element_request_pad(_tee, tee_src_pad_template, NULL, NULL);
            if(!_test_and_error(tee_eq_pad, "Engine: tee_eq_pad is NULL")) break;
            eq_pad = gst_element_get_static_pad(_eq_queue, "sink");
            if(!_test_and_error(eq_pad, "Engine: eq pad is NULL")) break;

            tee_app_pad = gst_element_request_pad(_tee, tee_src_pad_template, NULL, NULL);
            if(!_test_and_error(tee_app_pad, "Engine: tee_app_pad is NULL")) break;
            app_pad = gst_element_get_static_pad(_app_queue, "sink");
            if(!_test_and_error(app_pad, "Engine: app pad NULL")) break;

            s = gst_pad_link (tee_eq_pad, eq_pad);
            _test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee eq with eq");
            s = gst_pad_link (tee_app_pad, app_pad);
            _test_and_error_bool((s == GST_PAD_LINK_OK), "Engine: Cannot link tee app with app");


            // "input" of tee pad
            tee_pad = gst_element_get_static_pad(_tee, "sink");
        } else {
            tee_pad = gst_element_get_static_pad(_equalizer, "sink");
        }

        if(!_test_and_error(tee_pad, "Engine: Cannot create tee pad")) break;

        // tell the sink_bin, that the input of tee = input of sink_bin
        success = gst_element_add_pad(GST_ELEMENT(_audio_bin), gst_ghost_pad_new("sink", tee_pad));
        _test_and_error_bool(success, "Engine: cannot add pad to audio bin");

        // replace playbin sink with this bin
        g_object_set(G_OBJECT(_pipeline), "audio-sink", _audio_bin, NULL);


        if(with_app_sink){
            g_object_set (_app_queue,
                          "silent", TRUE,
                          NULL);

            g_object_set(_eq_queue,
                         "silent", TRUE,
                         NULL);


            GstCaps* audio_caps = gst_caps_from_string (AUDIO_CAPS);
            g_object_set (_app_sink,
                          "drop", TRUE,
                          "max-buffers", 1,
                          "caps", audio_caps,
                          "emit-signals", FALSE,
                          NULL);

            g_signal_connect (_app_sink, "new-buffer", G_CALLBACK (new_buffer), NULL);
        }

        g_signal_connect(_pipeline, "about-to-finish", G_CALLBACK(player_change_file), NULL);

        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_READY);
        success = true;

        break;
    } while (i);

    qDebug() << "Engine: constructor finished: " << success;
}

bool GStreamerBackend::set_uri(PlayBackend::BaseMediaObject *obj, bool startPlay)
{
    Q_UNUSED(startPlay)
    // Gstreamer needs an URI
    const gchar* uri = NULL;

    QString path;
    if (!obj->filePath().isEmpty() && !obj->fileName().isEmpty()) {
         path = QString("%1/%2").arg(obj->filePath()).arg(obj->fileName());
    } else {
        path = obj->filePath().isEmpty() ? obj->fileName() : obj->filePath();
    }

    if (_playing_stream) {
        uri = g_filename_from_utf8(path.toLocal8Bit(), path.toLocal8Bit().size(), NULL, NULL, NULL);
    }
    // no stream (not quite right because of mms, rtsp or other streams
    else if (!path.contains("://")) {
        uri = g_filename_to_uri(path.toLocal8Bit(), NULL, NULL);
    }
//    // fallback
//    else {
//        QString path = QString("%1/%2").arg(path).arg(obj->fileName());

//        qDebug()<<"Play path "<<path;

//        uri = g_filename_from_utf8(path.toUtf8(), path.toUtf8().size(), NULL, NULL, NULL);
//        if (!uri)
//            uri = g_filename_from_utf8(path.toLocal8Bit(), path.toLocal8Bit().size(), NULL, NULL, NULL);
//    }

    qDebug()<<"Play uri "<<uri;

    if (!uri)
        return NULL;

    // playing src
    _test_and_error(_pipeline, "Engine: pipeline is null");
    g_object_set(G_OBJECT(_pipeline), "uri", uri, NULL);

    qDebug() << "Engine:  set uri: " << uri;

    return true;
}
} //GStreamer
} //PlayBackend
} //PhoenixPlayer
