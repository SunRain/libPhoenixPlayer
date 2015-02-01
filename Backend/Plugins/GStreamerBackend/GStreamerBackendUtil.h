#ifndef GSTREAMERBACKEND_UTIL_H
#define GSTREAMERBACKEND_UTIL_H


#include <gst/gst.h>
#include <gst/gsturi.h>
#include <gst/app/gstappsink.h>
#include <gst/gstbuffer.h>
#include <gst/gstelement.h>

//#include <glib.h>

#include <cmath>
#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include <QObject>
#include <QDebug>
#include <QString>

#include "GStreamerBackend.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace GStreamer {

#define CHUNK_SIZE 1024
#define SAMPLE_RATE 44100 /* Samples per second we are sending */
#define AUDIO_CAPS "audio/x-raw-int,channels=2,rate=%d,signed=(boolean)true,width=16,depth=16,endianness=BYTE_ORDER"

#define LOOKUP_LOG(x) log_10[(int) (20000 * x)]

float log_10[20001];
float lo_128[128];

static GStreamerBackend* gst_obj_ref;
bool __start_at_beginning = false;
int __start_pos_beginning = 0;


gboolean player_change_file(GstBin* pipeline, void* app) {
    Q_UNUSED(pipeline);
    Q_UNUSED(app);
    gst_obj_ref->set_about_to_finish();
    return true;
}


static void new_buffer(GstElement* sink, void* data){

    Q_UNUSED(data);

    GstBuffer* buffer;
    g_signal_emit_by_name(sink, "pull-buffer", &buffer);
    if(!buffer) return;

    gst_obj_ref->set_buffer(buffer);
}



static gboolean show_position(GstElement* pipeline) {
    if (!__start_at_beginning)
        return false;

    gint64 pos;

    GstFormat fmt = GST_FORMAT_TIME;
    gst_element_query_position(pipeline, &fmt, &pos);

    if (gst_obj_ref
            && gst_obj_ref->getPlaybackState () == PhoenixPlayer::Common::PlaybackPlaying
            /*gst_obj_ref->getState() == STATE_PLAY*/) {
        gst_obj_ref->set_cur_position((quint32)(pos / 1000000000)); // sec
    }
    return true;
}

static gboolean bus_state_changed(GstBus *bus, GstMessage *msg,
                                  void *user_data) {

    (void) bus;
    (void) user_data;

    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        if (gst_obj_ref) {
            qDebug() << "Engine: Track finished";
            gst_obj_ref->set_track_finished();
        }
        break;
    case GST_MESSAGE_ERROR:
        GError *err;
        gst_message_parse_error(msg, &err, NULL);
        qDebug() << "Engine: GST_MESSAGE_ERROR: " << err->message << ": "
                 << GST_MESSAGE_SRC_NAME(msg);
        gst_obj_ref->set_track_finished();
        g_error_free(err);
        break;
    case GST_MESSAGE_ASYNC_DONE:
        if (__start_at_beginning == false) {
            __start_at_beginning = true;
            gst_obj_ref->setPosition(0);
        }
        break;
    default:
        gst_obj_ref->state_changed();
        break;
    }
    return true;
}


bool _test_and_error(void* element, QString errorstr){
    if(!element){
        qDebug() << errorstr;
        return false;
    }
    return true;
}

bool _test_and_error_bool(bool b, QString errorstr){
    if(!b){
        qDebug() << errorstr;
        return false;
    }
    return true;
}

} //GStreamer
} //PlayBackend
} //PhoenixPlayer

#endif // GSTREAMERBACKEND_UTIL_H
