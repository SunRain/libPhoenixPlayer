#include "PulseAudioOutput.h"

#include <QDebug>

extern "C"{
#include <pulse/error.h>
}

#include "OutPut/IOutPut.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {
namespace OutPut {
namespace PulseAudioOutput {

PulseAudioOutput::PulseAudioOutput(QObject *parent) :
    IOutPut(parent)
{
    m_connection = nullptr;
}

PulseAudioOutput::~PulseAudioOutput()
{
    uninitialize ();
}

bool PulseAudioOutput::initialize(quint32 srate, int chan, AudioParameters::AudioFormat f)
{
    qDebug()<<Q_FUNC_INFO<<QString("initialize for srate = [%1], chan = [%2], AudioFormat = [%3]")
              .arg (srate).arg (chan).arg (f);

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
    default:
        ss.format = PA_SAMPLE_S16LE;
    }

    ss.channels = chan;
    ss.rate = srate;
    int error;
    m_connection = pa_simple_new(NULL, // Use the default server.
                                 "PhoenixPlayer",               // Our application's name.
                                 PA_STREAM_PLAYBACK,
                                 NULL,                          // Use the default device.
                                 "Music",                       // Description of our stream.
                                 (const pa_sample_spec *)&ss,   // Our sample format.
                                 NULL,                          // Use default channel map
                                 NULL,                          // Use default buffering attributes.
                                 &error                         // Error code.
                                );
    if (!m_connection) {
        qWarning()<<Q_FUNC_INFO<<"pa_simple_new() failed: "<<pa_strerror(error);
        return false;
    }
//    Output::configure(freq, chan, format);
    IOutPut::configure (srate, chan, f);
    return true;
}

qint64 PulseAudioOutput::latency()
{
    if (!m_connection)
        return 0;
    int error = 0;
    qint64 delay =  pa_simple_get_latency(m_connection, &error)/1000;
    if (error) {
        qWarning()<<Q_FUNC_INFO<<pa_strerror (error);
        delay = 0;
    }
    return delay;
}

qint64 PulseAudioOutput::writeAudio(unsigned char *data, qint64 maxSize)
{
    int error;
    if (!m_connection)
        return -1;
    int i = 0;
    if ((i = pa_simple_write(m_connection, data, maxSize, &error)) < 0) {
        qWarning()<<Q_FUNC_INFO<<"pa_simple_write() failed: "<<pa_strerror(error);
        return -1;
    }
    return maxSize;
}

void PulseAudioOutput::drain()
{
    int error;
    if (m_connection)
        pa_simple_drain(m_connection, &error);
}

void PulseAudioOutput::reset()
{
    int error;
    if (m_connection)
        pa_simple_flush(m_connection, &error);
}

void PulseAudioOutput::uninitialize()
{
    if (m_connection)
    {
        qDebug()<<Q_FUNC_INFO<<"closing connection";
        pa_simple_free(m_connection);
        m_connection = 0;
    }
}


} //PulseAudioOutput
} //OutPut
} //PhoenixPlayer
