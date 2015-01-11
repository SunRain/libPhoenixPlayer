/* GSTEngine.cpp */

/* Copyright (C) 2012  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "HelperStructs/MetaData.h"
#include "HelperStructs/Helper.h"
#include "HelperStructs/Tagging/id3.h"
#include "HelperStructs/Equalizer_presets.h"
#include "Engine/Engine.h"
#include "Engine/GStreamer/GSTEngine.h"
#include "Engine/GStreamer/GSTEngineHelper.h"

#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <glib.h>


#include <string>
#include <vector>
#include <cmath>
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include <QObject>
#include <QDate>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <qplugin.h>

using namespace std;



/*****************************************************************************************/
/* Engine */
/*****************************************************************************************/

void _calc_log10_lut(){
    for(int i=0; i<=20000; i++){
        log_10[i] = log10(i / 20000.0f);
    }

    for(int i=0; i<128; i++){
        lo_128[i] = i*128.0f;
    }
}

GST_Engine::GST_Engine() {

    _caps.is_parsed = false;
    _calc_log10_lut();
    _settings = CSettingsStorage::getInstance();
    _name = "GStreamer Backend";
    _state = STATE_STOP;

    _seconds_started = 0;
    _seconds_now = 0;
    _scrobbled = false;
    _track_finished = true;

    _playing_stream = false;
    _sr_active = false;
    _sr_wanna_record = false;

    _gapless_track_available = false;
    _stream_recorder = new StreamRecorder();

   /* _timer = new QTimer(this);
    _timer->setInterval(10);

    connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));*/


    connect(_stream_recorder, SIGNAL(sig_initialized(bool)), this, SLOT(sr_initialized(bool)));
    connect(_stream_recorder, SIGNAL(sig_stream_ended()), this,
            SLOT(sr_ended()));
    connect(_stream_recorder, SIGNAL(sig_stream_not_valid()), this,
            SLOT(sr_not_valid()));

    //_timer->start();
}

GST_Engine::~GST_Engine() {

    qDebug() << "Engine: close engine... ";

    _settings->updateLastTrack();

    if (_bus)
        gst_object_unref (_bus);

    if (_pipeline) {
        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
        gst_object_unref (GST_OBJECT(_pipeline));}

    gst_obj_ref = 0;
}



void GST_Engine::init_play_pipeline() {

    bool success = false;
    bool with_app_sink = true;
    int i;

    i = 0;

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
        }

        else {
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




void GST_Engine::init() {

    gst_init(0, 0);

    _stream_recorder->init();

    init_play_pipeline();

}

void GST_Engine::psl_gapless_track(const MetaData& md) {
    //qDebug() << "Engine: Gapless track " << md.title;
    _md_gapless = md;
    _gapless_track_available = true;
}

void GST_Engine::changeTrack(const QString& filepath, int pos_sec,
        bool start_play) {
    MetaData md;
    md.filepath = filepath;
    if (!ID3::getMetaDataOfFile(md)) {
        stop();
        return;
    }
    changeTrack(md, pos_sec, start_play);
}

void GST_Engine::changeTrack(const MetaData& md, int pos_sec, bool start_play) {

    _caps.is_parsed = false;

    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);
    gst_obj_ref = this;

    _last_track = _settings->getLastTrack();

    if (!_track_finished || (md.filepath != _md_gapless.filepath) || true) {
        stop();
        _meta_data = md;
        // when stream ripper, do not start playing
        _playing_stream = Helper::is_www(md.filepath);

        bool success = set_uri(md, start_play);
        if (!success)
            return;

        g_timeout_add(500, (GSourceFunc) show_position, _pipeline);

        _gapless_track_available = false;
        //emit wanna_gapless_track();
        emit total_time_changed_signal(_meta_data.length_ms);
        emit timeChangedSignal(pos_sec);
    }

    else {
        _meta_data = _md_gapless;
        _gapless_track_available = false;

        emit wanna_gapless_track();
        emit total_time_changed_signal(_meta_data.length_ms);

        gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);
    }

    _seconds_started = 0;
    _seconds_now = 0;
    _scrobbled = false;
    _track_finished = false;

    //if(md.filepath == _md_gapless.filepath) return;

    if (pos_sec > 0) {
        __start_pos_beginning = pos_sec;
        __start_at_beginning = false;
    }

    else {
        __start_at_beginning = true;
    }

    if (start_play)
        play(pos_sec);

    // pause if streamripper is not active
    else if (!start_play && !(_playing_stream && _sr_active))
        pause();

}

bool GST_Engine::set_uri(const MetaData& md, bool& start_play) {
    // Gstreamer needs an URI
    const gchar* uri = NULL;

    // stream && streamripper active
    if (_playing_stream && _sr_active) {

        int max_tries = 10;

        QString filepath = _stream_recorder->changeTrack(md, max_tries);
        if (filepath.size() == 0) {
            qDebug() << "Engine: Stream Ripper Error: Could not get filepath";
            return false;
        }

        if (md.radio_mode == RADIO_LFM) {
            uri = g_filename_to_uri(
                    g_filename_from_utf8(filepath.toUtf8(),
                            filepath.toUtf8().size(), NULL, NULL, NULL), NULL,
                            NULL);
        }

        else {
            uri = g_filename_from_utf8(md.filepath.toUtf8(),
                    md.filepath.toUtf8().size(), NULL, NULL, NULL);
        }

        start_play = false;
    }

    // stream, but don't wanna record
    else if (_playing_stream && !_sr_active) {
        uri = g_filename_from_utf8(md.filepath.toUtf8(),
                md.filepath.toUtf8().size(), NULL, NULL, NULL);
    }

    // no stream (not quite right because of mms, rtsp or other streams
    else if (!md.filepath.contains("://")) {

        uri = g_filename_to_uri(md.filepath.toLocal8Bit(), NULL, NULL);
    }

    else {
        uri = g_filename_from_utf8(md.filepath.toUtf8(),
                md.filepath.toUtf8().size(), NULL, NULL, NULL);
    }

    if (!uri)
        return NULL;

    // playing src
    _test_and_error(_pipeline, "Engine: pipeline is null");
        g_object_set(G_OBJECT(_pipeline), "uri", uri, NULL);

    qDebug() << "Engine:  set uri: " << uri;

    return true;
}

void GST_Engine::play(int pos_sec) {

    _track_finished = false;
    _state = STATE_PLAY;

    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

    gst_obj_ref = this;
}

void GST_Engine::stop() {

    _state = STATE_STOP;

    // streamripper, wanna record is set when record button is pressed
    if (_playing_stream && _sr_active) {
        qDebug() << "Engine: stop... Playing stream";
        _stream_recorder->stop(!_sr_wanna_record);
    }

    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_NULL);

    _track_finished = true;
}

void GST_Engine::pause() {
    _state = STATE_PAUSE;
    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PAUSED);
}

void GST_Engine::setVolume(int vol) {

    _vol = vol;

    float vol_val = (float) (vol * 1.0f / 100.0f);

    g_object_set(G_OBJECT(_pipeline), "volume", vol_val, NULL);


}

void GST_Engine::load_equalizer(vector<EQ_Setting>& vec_eq_settings) {

    emit eq_presets_loaded(vec_eq_settings);
}

void GST_Engine::jump(int where, bool percent) {

    gint64 new_time_ms, new_time_ns;
    if (where < 0)
        where = 0;

    if (percent) {
        if (where > 100)
            where = 100;
        double p = where / 100.0;
        _seconds_started = (int) (p * _meta_data.length_ms) / 1000;
        new_time_ms = (quint64)(p * _meta_data.length_ms); // msecs
    }

    else {
        if (where > _meta_data.length_ms / 1000)
            where = _meta_data.length_ms / 1000;
        _seconds_started = where;
        new_time_ms = where * 1000;
    }

    new_time_ns = new_time_ms * GST_MSECOND;

    if (!gst_element_seek_simple(_pipeline, GST_FORMAT_TIME,
            (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
            new_time_ns)) {
        qDebug() << "seeking failed ";
    }
}

void GST_Engine::eq_changed(int band, int val) {

    double new_val = 0;
    new_val = val * 1.0;
    if (val > 0) {
        new_val = val * 0.25;
    }

    else
        new_val = val * 0.75;

    char band_name[6];
    sprintf(band_name, "band%d", band);
    band_name[5] = '\0';

    g_object_set(G_OBJECT(_equalizer), band_name, new_val, NULL);
}

void GST_Engine::eq_enable(bool) {

}

void GST_Engine::psl_calc_level(bool b){

        g_object_set (_app_sink,
                               "emit-signals", b,
                               NULL);

}

void GST_Engine::state_changed() {

}

void GST_Engine::set_cur_position(quint32 pos_sec) {

    if ((quint32) _seconds_now == pos_sec)
        return;
    _seconds_now = pos_sec;
    int playtime = _seconds_now - _seconds_started;

    if (!_scrobbled
            && (playtime >= 15 || playtime == _meta_data.length_ms / 2000)) {

        emit scrobble_track(_meta_data);
        _scrobbled = true;
    }

    _last_track->id = _meta_data.id;
    _last_track->filepath = _meta_data.filepath;
    _last_track->pos_sec = pos_sec;

    emit timeChangedSignal(_seconds_now);
}

void GST_Engine::set_track_finished() {

    if (_sr_active && _playing_stream) {
        _stream_recorder->stop(!_sr_wanna_record);
    }

    emit track_finished();
    _track_finished = true;
}

void GST_Engine::set_about_to_finish() {

    //qDebug() << "Engine: about to finish " << _gapless_track_available;
    if (_gapless_track_available && 0) {

        bool b = true;
        set_uri(_md_gapless, b);
        _gapless_track_available = false;
        _track_finished = true;
        emit track_finished();
    }
}


float f_channel[2];
#define SCALE_SHORT 0.000000004f
#define BUFFER_SIZE 4096

void GST_Engine::emit_buffer(float inv_arr_channel_elements, float scale){

    for(int i=0; i<_caps.channels; i++){
        float val = f_channel[i] * inv_arr_channel_elements * scale;
        if(val > 1.0f) val = 1.0f;
        f_channel[i] = 10.0f * LOOKUP_LOG(val);
    }

    if(_caps.channels >= 2)
        emit sig_level(f_channel[0], f_channel[1]);

    else if(_caps.channels == 1)
        emit sig_level(f_channel[0], -100.0f);

    f_channel[0] = 0;
    f_channel[1] = 0;
}


void GST_Engine::set_buffer(GstBuffer* buffer){

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

     emit_buffer(inv_arr_channel_elements, scale);

     gst_buffer_unref(buffer);

}

int GST_Engine::getState() {
    return _state;
}

QString GST_Engine::getName() {
    return _name;
}

void GST_Engine::record_button_toggled(bool b) {
    _sr_wanna_record = b;
}

void GST_Engine::psl_sr_set_active(bool b) {
    _sr_active = b;
}

void GST_Engine::psl_new_stream_session() {
    _stream_recorder->set_new_stream_session();
}

void GST_Engine::sr_initialized(bool b) {

    if (b)
        play();

}

void GST_Engine::sr_ended() {
    //emit track_finished();
}

void GST_Engine::sr_not_valid() {
    qDebug() << "Engine: Stream not valid.. Next file";
    emit track_finished();
}

Q_EXPORT_PLUGIN2(sayonara_gstreamer, GST_Engine)
