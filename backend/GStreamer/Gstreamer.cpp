#include "Gstreamer.h"
#include "Util.h"

static void _on_eos(GstBus *, GstMessage *, gpointer userData)
{
    PhoenixPlayerCore::Gstreamer *obj = reinterpret_cast<PhoenixPlayerCore::Gstreamer *>(userData);
    obj->_finish();
}

static void _on_error(GstBus *, GstMessage *msg, gpointer userData)
{
    gchar *debug;
    GError *err;

    gst_message_parse_error(msg, &err, &debug);
    g_free(debug);

    PhoenixPlayerCore::Gstreamer *obj = reinterpret_cast<PhoenixPlayerCore::Gstreamer *>(userData);
    obj->_emitError(err->message);
    obj->_fail();

    g_error_free(err);
}

static void _on_about_to_finish(GstElement *playbin, gpointer userData)
{
    PhoenixPlayerCore::Gstreamer *obj = reinterpret_cast<PhoenixPlayerCore::Gstreamer *>(userData);

//    gchar *uri_before;
//    g_object_get(playbin, "uri", &uri_before, NULL);

//    obj->_crossfadingPrepare();
//    obj->_emitAboutToFinish();

//    gchar *uri_after = g_filename_to_uri(QFileInfo(obj->currentMedia()).absoluteFilePath().toUtf8().constData(), NULL, NULL);

//    if (g_strcmp0(uri_before, uri_after) == 0) // uri hasn't changed
//        obj->_crossfadingCancel();

//    g_free(uri_before);
//    g_free(uri_after);
}


PhoenixPlayerCore::Gstreamer::Gstreamer(QObject *parent) : QObject(parent)
{

}

PhoenixPlayerCore::Gstreamer::~Gstreamer()
{
    stop();
    gst_object_unref(m_playbin);
}

void PhoenixPlayerCore::Gstreamer::init()
{
    gst_init(0, 0);
    init_play_pipeline();
}

void PhoenixPlayerCore::Gstreamer::set_buffer(GstBuffer *)
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

    emit_buffer(inv_arr_channel_elements, scale);

    gst_buffer_unref(buffer);
}

void PhoenixPlayerCore::Gstreamer::play(quint64 startMs)
{
//    _track_finished = false;
//    _state = Common::PlaybackState::PlaybackPlaying;

//    gst_element_set_state(GST_ELEMENT(_pipeline), GST_STATE_PLAYING);

//    gst_obj_ref = this;
    GstState gstState;
    gst_element_get_state(m_playbin, &gstState, NULL, 0);
    if (gstState != GST_STATE_PLAYING) {
        gst_element_set_state(m_playbin, GST_STATE_PLAYING);
    }
    _state = Common::PlaybackState::PlaybackPlaying;

    m_timer->start(100);

    if (startMs > 0)
        setPosition(startMs);
}

void PhoenixPlayerCore::Gstreamer::stop()
{
    _state = Common::PlaybackStopped;

    m_crossfading = FALSE;

        gst_element_set_state(GST_ELEMENT(m_playbin), GST_STATE_NULL);

        //        _track_finished = true;
}

void PhoenixPlayerCore::Gstreamer::pause()
{
    _state = Common::PlaybackPaused;
    gst_element_set_state(GST_ELEMENT(m_playbin), GST_STATE_PAUSED);

    m_timer->stop();
    checkStatus();
}

void PhoenixPlayerCore::Gstreamer::setVolume(int vol)
{
    _vol = vol;

    float vol_val = (float) (vol * 1.0f / 100.0f);

    g_object_set(G_OBJECT(m_playbin), "volume", vol_val, NULL);
    //    g_object_set(m_playbin, "volume", qBound(0.0, volume, 1.0), NULL);
}

void PhoenixPlayerCore::Gstreamer::setPosition(quint64 posMs)
{
    ///
    /// TODO:由其他代码完成时间转换功能

//    gint64 newTimeMs, newTimeNs;
//    if (pos < 0)
//        pos = 0;

//    if (percent) {
//        if (pos > 100)
//            pos = 100;
//        double p = pos / 100.0;
//        _seconds_started = (int) (p * _meta_data.length_ms) / 1000;
//        newTimeMs = (quint64)(p * _meta_data.length_ms); // msecs
//    }

//    else {
//        if (pos > _meta_data.length_ms / 1000)
//            pos = _meta_data.length_ms / 1000;
//        _seconds_started = pos;
//        newTimeMs = pos * 1000;
//    }

    gint64 newTimeNs = posMs * GST_MSECOND;

//    gst_element_seek(m_playbin, 1.0,
//                             GST_FORMAT_TIME,
//                             GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
//                             GST_SEEK_TYPE_SET, pos * m_durationNsec,
//                             GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);

    if (!gst_element_seek_simple(m_playbin, GST_FORMAT_TIME,
                                 (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SKIP),
                                 newTimeNs)) {
        qDebug() << "seeking failed ";
    }
}

void PhoenixPlayerCore::Gstreamer::changeMedia(PhoenixPlayerCore::BaseMediaObject *obj, quint64 startMs, bool startPlay)
{
//    _caps.is_parsed = false;

    gst_element_set_state(GST_ELEMENT(m_playbin), GST_STATE_NULL);
//    gst_obj_ref = this;

    //    _last_track = _settings->getLastTrack();

    if (!_track_finished /*|| (md.filepath != _md_gapless.filepath)*/ || true) {
        stop();
        //        _meta_data = md;
        // when stream ripper, do not start playing
        _playing_stream = (obj->mediaType() == Common::TypeStream);/*Helper::is_www(md.filepath);*/

        bool success = set_uri(obj, startPlay);
        if (!success)
            return;

//        g_timeout_add(500, (GSourceFunc) show_position, m_playbin);

        _gapless_track_available = false;
        //emit wanna_gapless_track();
        ///TODO: 添加信号
        //        emit total_time_changed_signal(_meta_data.length_ms);
        //        emit timeChangedSignal(startSec);
    } else {
        //        _meta_data = _md_gapless;
        _gapless_track_available = false;

        ///TODO: 添加信号
        //        emit wanna_gapless_track();
        //        emit total_time_changed_signal(_meta_data.length_ms);

        gst_element_set_state(GST_ELEMENT(m_playbin), GST_STATE_PLAYING);
    }

//    _seconds_started = 0;
//    _seconds_now = 0;
//    _scrobbled = false;
//    _track_finished = false;


//    if (startMs > 0) {
//        __start_pos_beginning = startMs;
//        __start_at_beginning = false;
//    }

//    else {
//        __start_at_beginning = true;
//    }

    if (startPlay) {
        play(startMs);
    } else if (!startPlay /*&& !(_playing_stream && _sr_active)*/) { // pause if streamripper is not active
        pause();
    }
}

void PhoenixPlayerCore::Gstreamer::checkStatus()
{
//    GstState gstState;
//    gst_element_get_state(m_playbin, &gstState, NULL, 0);
//    N::PlaybackState state = fromGstState(gstState);
//    if (m_oldState != state)
//        emit stateChanged(m_oldState = state);

//    if (state == N::PlaybackPlaying || state == N::PlaybackPaused) {
//        // duration may change for some reason
//        // TODO use DURATION_CHANGED in gstreamer1.0
//        GstFormat format = GST_FORMAT_TIME;
//        gboolean res = gst_element_query_duration(m_playbin, &format, &m_durationNsec);
//        if (!res || format != GST_FORMAT_TIME)
//            m_durationNsec = 0;
//    }

//    if (m_posponedPosition >= 0 && m_durationNsec > 0) {
//        setPosition(m_posponedPosition);
//        m_posponedPosition = -1;
//        emit positionChanged(m_posponedPosition);
//    } else {
//        qreal pos;
//        gint64 gstPos = 0;

//        if (!hasMedia() || m_durationNsec <= 0) {
//            pos = -1;
//        } else {
//            GstFormat format = GST_FORMAT_TIME;
//            gboolean res = gst_element_query_position(m_playbin, &format, &gstPos);
//            if (!res || format != GST_FORMAT_TIME)
//                gstPos = 0;
//            pos = (qreal)gstPos / m_durationNsec;
//        }

//        if (m_oldPosition != pos) {
//            if (m_oldPosition > pos)
//                m_crossfading = FALSE;
//            m_oldPosition = pos;
//            emit positionChanged(m_crossfading ? 0 : m_oldPosition);
//        }

//        emit tick(m_crossfading ? 0 : gstPos / 1000000);
//    }

#if defined Q_WS_WIN || defined Q_WS_MAC
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_playbin));
    GstMessage *msg = gst_bus_pop_filtered(bus, GstMessageType(GST_MESSAGE_EOS | GST_MESSAGE_ERROR));
    if (msg) {
        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            _on_eos(bus, msg, this);
            break;
        case GST_MESSAGE_ERROR:
            _on_error(bus, msg, this);
            break;
        default:
            break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
#endif

//    qreal vol = volume();
//    if (qAbs(m_oldVolume - vol) > 0.0001) {
//        m_oldVolume = vol;
//        emit volumeChanged(vol);
//    }

    if ( _state = Common::PlaybackStopped)
        m_timer->stop();
}

void PhoenixPlayerCore::Gstreamer::init_play_pipeline()
{
    bool success = false;
    bool with_app_sink = true;
    int i = 0;

    // eq -> autoaudiosink is packaged into a bin
    do {
        // create equalizer element
        m_playbin = gst_element_factory_make("playbin2", "player");

        _test_and_error(m_playbin, "Engine: Pipeline sucks");

//        g_signal_connect(m_playbin, "about-to-finish", G_CALLBACK(player_change_file), NULL);
g_signal_connect(m_playbin, "about-to-finish", G_CALLBACK(_on_about_to_finish), this);


//        _bus = gst_pipeline_get_bus(GST_PIPELINE(m_playbin));
//#if !defined Q_WS_WIN && !defined Q_WS_MAC
        GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_playbin));

        gst_bus_add_signal_watch(bus);

        g_signal_connect(bus, "message::error", G_CALLBACK(_on_error), this);
        g_signal_connect(bus, "message::eos", G_CALLBACK(_on_eos), this);
        gst_object_unref(bus);
//#endif

        _audio_bin = gst_bin_new("audio-bin");
        _equalizer = gst_element_factory_make("equalizer-10bands", "equalizer");


        _decodebin = gst_element_factory_make("decodebin2", "decoder");
        _audio_sink = gst_element_factory_make("autoaudiosink", "autoaudiosink");

        _eq_queue = gst_element_factory_make("queue", "eq_queue");
        _tee = gst_element_factory_make("tee", "tee");
        _app_queue = gst_element_factory_make("queue", "app_queue");
        _app_sink = gst_element_factory_make("appsink", "app_sink");


//        if(!_test_and_error(_bus, "Engine: Something went wrong with the bus")) //break;
//        if(!_test_and_error(bus, "Engine: Something went wrong with the bus"))  break;
        //if(!_test_and_error(_gio_src, "Engine: Giosrc fail")) break;
        if(!_test_and_error(_decodebin, "Engine: decodebin fail")) break;
        if(!_test_and_error(_audio_bin, "Engine: Bin cannot be created")) break;
        if(!_test_and_error(_tee, "Engine: Tee cannot be created")) break;
        if(!_test_and_error(_equalizer, "Engine: Equalizer cannot be created")) break;
        if(!_test_and_error(_eq_queue, "Engine: Equalizer cannot be created")) break;
        if(!_test_and_error(_audio_sink, "Engine: Audio Sink cannot be created")) break;
        if(!_test_and_error(_app_queue, "Engine: Queue cannot be created")) break;
        if(!_test_and_error(_app_sink, "Engine: App Sink cannot be created")) break;

//        gst_bus_add_watch(_bus, bus_state_changed, this);

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
        g_object_set(G_OBJECT(m_playbin), "audio-sink", _audio_bin, NULL);


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

            g_signal_connect (_app_sink, "new-buffer", G_CALLBACK (new_buffer), this);
        }


        gst_element_set_state(GST_ELEMENT(m_playbin), GST_STATE_READY);
        success = true;

        break;
    } while (i);

    qDebug() << "Engine: constructor finished: " << success;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(checkStatus()));
}

bool PhoenixPlayerCore::Gstreamer::set_uri(PhoenixPlayerCore::BaseMediaObject *obj, const bool &startPlay)
{
    // Gstreamer needs an URI
    const gchar* uri = NULL;

    if (_playing_stream) {
        uri = g_filename_from_utf8(obj->filePath().toUtf8(), obj->filePath().toUtf8().size(), NULL, NULL, NULL);
        if (!uri)
            uri = g_filename_from_utf8(obj->fileName().toUtf8(), obj->fileName().toUtf8().size(), NULL, NULL, NULL);
    }
    // no stream (not quite right because of mms, rtsp or other streams
    else if (!obj.contains("://")) {
        uri = g_filename_to_uri(obj->filePath().toLocal8Bit(), NULL, NULL);
        if (!uri)
            uri = g_filename_to_uri(obj->fileName().toLocal8Bit(), NULL, NULL);
    }
    // fallback
    else {
        QString path = QString("%1/%2").arg(obj->filePath()).arg(obj->fileName());
        uri = g_filename_from_utf8(path.toUtf8(), path.toUtf8().size(), NULL, NULL, NULL);
        if (!uri)
            uri = g_filename_from_utf8(path.toLocal8Bit(), path.toLocal8Bit().size(), NULL, NULL, NULL);
    }
    if (!uri)
        return NULL;

    // playing src
    _test_and_error(m_playbin, "Engine: pipeline is null");
    g_object_set(G_OBJECT(m_playbin), "uri", uri, NULL);

    qDebug() << "Engine:  set uri: " << uri;

    return true;
}

void PhoenixPlayerCore::Gstreamer::_finish()
{
    stop();
//    emit finished();
    //    emit stateChanged(m_oldState = N::PlaybackStopped);
}

void PhoenixPlayerCore::Gstreamer::_emitError(QString error)
{
    qDebug()<< error;

}

void PhoenixPlayerCore::Gstreamer::_fail()
{
//    if (!m_crossfading) // avoid thread deadlock
//        stop();
//    else
//        m_crossfading = FALSE;
//    emit mediaChanged(m_currentMedia = "");
//    emit failed();
//    emit stateChanged(m_oldState = N::PlaybackStopped);
}
