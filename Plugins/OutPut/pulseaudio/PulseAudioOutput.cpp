#include "PulseAudioOutput.h"

#include <QList>
#include <QDebug>

extern "C"{
#include <pulse/error.h>
}

#include "OutPut/IOutPut.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {
namespace OutPut {
namespace PulseAudioOutput {

static void info_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata)
{
    if(!i)
        return;

    if(userdata) {
        *(bool *) userdata = true;
    }
}

static void context_success_cb(pa_context *, int success, void *data)
{
    if(data) {
        *(bool *)data = success != 0;
    }
}

static void stream_success_cb(pa_stream *, int success, void *data)
{
    if(data) {
        *(bool *)data = success != 0;
    }
}

PulseAudioOutput::PulseAudioOutput(QObject *parent)
    : IOutPut(parent)
{    
    m_pa_channels.insert(AudioParameters::CHAN_NULL,            PA_CHANNEL_POSITION_INVALID);
    m_pa_channels.insert(AudioParameters::CHAN_FRONT_CENTER,    PA_CHANNEL_POSITION_MONO);
    m_pa_channels.insert(AudioParameters::CHAN_FRONT_LEFT,      PA_CHANNEL_POSITION_FRONT_LEFT);
    m_pa_channels.insert(AudioParameters::CHAN_FRONT_RIGHT,     PA_CHANNEL_POSITION_FRONT_RIGHT);
    m_pa_channels.insert(AudioParameters::CHAN_REAR_LEFT,       PA_CHANNEL_POSITION_REAR_LEFT);
    m_pa_channels.insert(AudioParameters::CHAN_REAR_RIGHT,      PA_CHANNEL_POSITION_REAR_RIGHT);
    m_pa_channels.insert(AudioParameters::CHAN_FRONT_CENTER,    PA_CHANNEL_POSITION_FRONT_CENTER);
    m_pa_channels.insert(AudioParameters::CHAN_LFE,             PA_CHANNEL_POSITION_LFE);
    m_pa_channels.insert(AudioParameters::CHAN_SIDE_LEFT,       PA_CHANNEL_POSITION_SIDE_LEFT);
    m_pa_channels.insert(AudioParameters::CHAN_SIDE_RIGHT,      PA_CHANNEL_POSITION_SIDE_RIGHT);
    m_pa_channels.insert(AudioParameters::CHAN_REAR_CENTER,     PA_CHANNEL_POSITION_REAR_CENTER);
}

PulseAudioOutput::~PulseAudioOutput()
{
    uninitialize ();
}

bool PulseAudioOutput::initialize(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &channels, AudioParameters::AudioFormat f)
{
    IOutPut::initialize(sampleRate, channels, f);

    if (!(m_loop = pa_mainloop_new())) {
        qWarning()<<Q_FUNC_INFO<<"unable to allocate a new main loop object";
        return false;
    }

    if (!(m_ctx = pa_context_new(pa_mainloop_get_api(m_loop), "PhoenixPlayer"))) {
        qWarning()<<Q_FUNC_INFO<<"unable to instantiate a new connection context";
        return false;
    }

    if (pa_context_connect(m_ctx, Q_NULLPTR, (pa_context_flags_t)0, Q_NULLPTR) < 0) {
        qWarning()<<Q_FUNC_INFO<<"unable to connect the context: "<<pa_strerror(pa_context_errno(m_ctx));
        return false;
    }

    //waiting for context connection
    pa_context_state_t context_state = PA_CONTEXT_UNCONNECTED;
    while ((context_state = pa_context_get_state(m_ctx)) != PA_CONTEXT_READY) {
        if (context_state == PA_CONTEXT_TERMINATED || context_state == PA_CONTEXT_FAILED) {
            qWarning()<<Q_FUNC_INFO<<"unable to connect the context: "<<pa_strerror(pa_context_errno(m_ctx));
            return false;
        }
        poll();
    }

    pa_sample_spec ss;

    switch (f) {
    case AudioParameters::PCM_S8:
        ss.format = PA_SAMPLE_U8;
        break;
    case AudioParameters::PCM_S16LE:
        ss.format = PA_SAMPLE_S16LE;
        break;
    case AudioParameters::PCM_S24LE:
        ss.format = PA_SAMPLE_S24_32LE;
        break;
    case AudioParameters::PCM_S32LE:
        ss.format = PA_SAMPLE_S32LE;
        break;
    case AudioParameters::PCM_FLOAT:
#ifdef PA_SAMPLE_FLOAT32NE
        ss.format = PA_SAMPLE_FLOAT32NE;
        break;
#endif
    default:
        ss.format = PA_SAMPLE_S16LE;
    }

    ss.channels = channels.count();
    ss.rate = sampleRate;

    pa_channel_map pa_map;
    pa_map.channels = channels.count();
    for (int i = 0; i < channels.count(); ++i) {
        pa_map.map[i] = m_pa_channels[channels.value(i)];
    }

    if (!(m_stream = pa_stream_new(m_ctx, "PhoenixPlayer", &ss, &pa_map))) {
        qWarning()<<Q_FUNC_INFO<<"unable to create stream: "<<pa_strerror(pa_context_errno(m_ctx));
        return false;
    }

    pa_buffer_attr attr;
    attr.maxlength = (uint32_t) -1;
    attr.tlength = (uint32_t) pa_usec_to_bytes((pa_usec_t)500 * 1000, &ss); //500 ms
    attr.prebuf = (uint32_t) -1;
    attr.minreq = (uint32_t) -1;
    attr.fragsize = attr.tlength;

    pa_stream_flags_t flags = pa_stream_flags_t(PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_AUTO_TIMING_UPDATE);

    if (pa_stream_connect_playback(m_stream, nullptr, &attr, flags, Q_NULLPTR, nullptr) < 0) {
        qWarning()<<Q_FUNC_INFO<<"unable to connect playback: "<<pa_strerror(pa_context_errno(m_ctx));
        return false;
    }
    //waiting for stream connection
    pa_stream_state_t stream_state = PA_STREAM_UNCONNECTED;
    while ((stream_state = pa_stream_get_state(m_stream)) != PA_STREAM_READY) {
        if (stream_state == PA_STREAM_FAILED || stream_state == PA_STREAM_TERMINATED) {
            qWarning()<<Q_FUNC_INFO<<"unable to connect playback: "<<pa_strerror(pa_context_errno(m_ctx));
            return false;
        }
        poll();
    }

    pa_context_set_subscribe_callback(m_ctx,
        [] (pa_context *ctx, pa_subscription_event_type t, uint32_t index, void *data){
              pa_operation *op;
              PulseAudioOutput *output = (PulseAudioOutput *)data;

              if (!output || !output->m_stream || index != pa_stream_get_index(output->m_stream) ||
                  (t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT | PA_SUBSCRIPTION_EVENT_NEW) &&
                   t != (PA_SUBSCRIPTION_EVENT_SINK_INPUT | PA_SUBSCRIPTION_EVENT_CHANGE))) {
                  return;
              }

              op = pa_context_get_sink_input_info(ctx, index, info_cb, nullptr);
              if (!op) {
                  qWarning()<<Q_FUNC_INFO<<"pa_context_get_sink_input_info failed: "
                             <<pa_strerror(pa_context_errno(ctx));
                  return;
              }
              pa_operation_unref(op);
        },
        this);

    bool success = false;
    pa_operation *op = pa_context_subscribe(m_ctx,
                                            PA_SUBSCRIPTION_MASK_SINK_INPUT,
                                            context_success_cb,
                                            &success);
    if (!process(op) || !success) {
        qWarning()<<Q_FUNC_INFO<<"pa_context_subscribe failed: "<<pa_strerror(pa_context_errno(m_ctx));
        return false;
    }
    success = false;
    op = pa_context_get_sink_input_info(m_ctx, pa_stream_get_index(m_stream), info_cb, &success);
    if (!process(op) || !success) {
        qWarning()<<Q_FUNC_INFO<<"pa_context_get_sink_input_info: "<<pa_strerror(pa_context_errno(m_ctx));
        return false;
    }
    return true;
}

qint64 PulseAudioOutput::latency()
{
    pa_usec_t usec;
    int negative;

    int r = pa_stream_get_latency(m_stream, &usec, &negative);
    return (r == PA_OK && negative == 0) ? (usec / 1000) : 0;
}

qint64 PulseAudioOutput::writeAudio(unsigned char *data, qint64 maxSize)
{
    while (!pa_stream_writable_size(m_stream) || !isReady()) {
        poll();
    }

    size_t length = qMin(size_t(maxSize), pa_stream_writable_size(m_stream));
    if (pa_stream_write(m_stream, data, length, nullptr, 0, PA_SEEK_RELATIVE) < 0) {
        qWarning()<<Q_FUNC_INFO<<"pa_stream_write failed: "<<pa_strerror(pa_context_errno(m_ctx));
        return -1;
    }
    return qint64(length);
}

void PulseAudioOutput::drain()
{
    pa_operation *op = pa_stream_drain(m_stream, stream_success_cb, nullptr);
    process(op);
}

void PulseAudioOutput::reset()
{
    pa_operation *op = pa_stream_flush(m_stream, stream_success_cb, nullptr);
    process(op);
}

void PulseAudioOutput::suspend()
{
    pa_operation *op = pa_stream_cork(m_stream, 1, stream_success_cb, nullptr);
    process(op);
}

void PulseAudioOutput::resume()
{
    pa_operation *op = pa_stream_cork(m_stream, 0, stream_success_cb, nullptr);
    process(op);
}

void PulseAudioOutput::uninitialize()
{
    if (m_stream) {
        pa_stream_disconnect(m_stream);
        pa_stream_unref(m_stream);
        m_stream = nullptr;
    }

    if (m_ctx) {
        pa_context_disconnect(m_ctx);
        pa_context_unref(m_ctx);
        m_ctx = nullptr;
    }

    if(m_loop) {
        pa_mainloop_free(m_loop);
        m_loop = nullptr;
    }
}

bool PulseAudioOutput::isReady() const
{
    return m_ctx && m_stream &&
           pa_context_get_state(m_ctx) == PA_CONTEXT_READY &&
            pa_stream_get_state(m_stream) == PA_STREAM_READY;
}

void PulseAudioOutput::poll()
{
    pa_mainloop_prepare(m_loop, -1);
    pa_mainloop_poll(m_loop);
    pa_mainloop_dispatch(m_loop);
}

bool PulseAudioOutput::process(pa_operation *op)
{
    if (!op) {
        return false;
    }

    pa_operation_state_t state;
    while ((state = pa_operation_get_state(op)) != PA_OPERATION_DONE && isReady()) {
        poll();
    }

    pa_operation_unref(op);
    return (state == PA_OPERATION_DONE) && isReady();
}


} //PulseAudioOutput
} //OutPut
} //PhoenixPlayer
