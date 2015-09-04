#include "OutputThread.h"

#include <QDebug>

#include "PluginLoader.h"
#include "PluginHost.h"
#include "OutPut/IOutPut.h"
#include "AudioParameters.h"
#include "Recycler.h"
#include "Buffer.h"
#include "SingletonPointer.h"
#include "EqualizerMgr.h"

#include "Backend/BaseVolume.h"
#include "Backend/SoftVolume.h"
#include "Backend/BaseVisual.h"

#include "StateHandler.h"
#include "PhoenixBackend_global.h"

extern "C" {
#include "equ/iir.h"
}

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

//static functions
static inline void s8_to_s16(qint8 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] << 8;
    return;
}

static inline void s24_to_s16(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 8;
    return;
}

static inline void s32_to_s16(qint32 *in, qint16 *out, qint64 samples)
{
    for(qint64 i = 0; i < samples; ++i)
        out[i] = in[i] >> 16;
    return;
}

OutputThread::OutputThread(QObject *parent, BaseVisual *v)
    : QThread(parent)
    , m_visual(v)
{
    m_frequency = 0;
    m_output = nullptr;
    m_totalWritten = 0;
    m_currentMilliseconds = -1;
    m_bytesPerMillisecond = 0;
    m_userStop = false;
    m_finish = false;
    m_kbps = 0;
    m_skip = false;
    m_pause = false;
    m_prev_pause = false;
    m_useEq = false;
    m_muted = false;

    m_pluginHost = nullptr;

    m_visBuffer = nullptr;
    m_visBufferSize = 0;

    m_audioParameters = new AudioParameters(0, 0, AudioParameters::PCM_UNKNOWM, this);

    m_handler = StateHandler::instance ();

    m_pluginLoader = PluginLoader::instance ();

    m_eq = EqualizerMgr::instance ();

    connect (m_eq, &EqualizerMgr::changed, this, &OutputThread::updateEQ);
}

OutputThread::~OutputThread()
{
    if (m_pluginHost) {
        m_pluginHost->unLoad ();
    }
    if (m_audioParameters) {
        m_audioParameters->deleteLater();
        m_audioParameters = nullptr;
    }
    if (m_visBuffer) {
        delete[] m_visBuffer;
        m_visBuffer = nullptr;
    }
    if (m_visual)
        m_visual = nullptr;
}

bool OutputThread::initialize(quint32 freq, int chan, AudioParameters::AudioFormat format)
{
    qDebug()<<Q_FUNC_INFO<<"=========";

    if (!m_output) {
        m_output = m_pluginLoader->getCurrentOutPut ();
    }
    if (!m_output) {
        qCritical()<<Q_FUNC_INFO<<"Can't find output!!";
        return false;
    }
    if (!m_output->initialize (freq, chan, format)) {
        qCritical()<<Q_FUNC_INFO<<"Can't init output";
        m_pluginHost = m_pluginLoader->getCurrentPluginHost (Common::PluginOutPut);
        m_pluginHost->unLoad ();
        m_output = nullptr;
        return false;
    }
    if (m_audioParameters) {
        m_audioParameters->deleteLater ();
        m_audioParameters = nullptr;
    }

    m_audioParameters = new AudioParameters(m_output->audioParameters (), this);

    qDebug()<<Q_FUNC_INFO<<"Current audio info "<<m_audioParameters->parametersInfo ();

    m_bytesPerMillisecond = freq * chan * AudioParameters::sampleSize(format) / 1000;
    m_recycler.configure(freq, chan, format); //calculate output buffer size

    if (m_visBuffer) {
        delete[] m_visBuffer;
    }
    m_visBufferSize = BUFFER_PERIOD * 2 * chan; //16-bit samples
    if (m_audioParameters->format () != AudioParameters::PCM_S16LE)
        m_visBuffer = new unsigned char [m_visBufferSize];

    updateEQ ();
    return true;
}

void OutputThread::pause()
{
    m_mutex.lock ();
    m_pause = !m_pause;
    m_mutex.unlock ();
    PlayState s = m_pause ? PlayState::Paused : PlayState::Playing;
    dispatch(s);
}

void OutputThread::stop()
{
    m_userStop = true;
}

void OutputThread::setMuted(bool muted)
{
    m_muted = muted;
}

void OutputThread::finish()
{
    m_finish = true;
}

void OutputThread::seek(qint64 pos, bool reset)
{
    m_totalWritten = pos * m_bytesPerMillisecond;
    m_currentMilliseconds = -1;
    m_skip = this->isRunning ()&& reset;
}

Recycler *OutputThread::recycler()
{
    return &m_recycler;
}

QMutex *OutputThread::mutex()
{
    return  &m_mutex;
}

AudioParameters *OutputThread::audioParameters()
{
    return m_audioParameters;
}

//BaseVolume *OutputThread::baseVolume()
//{
//    return m_volume;
//}

void OutputThread::run()
{
    qDebug()<<Q_FUNC_INFO<<"===============";

    m_mutex.lock ();
    if (!m_bytesPerMillisecond) {
        qWarning()<<Q_FUNC_INFO<<"invalid audio parameters";
        m_mutex.unlock ();
        return;
    } else {
        qDebug()<<Q_FUNC_INFO<<"m_bytesPerMillisecond "<<m_bytesPerMillisecond;
    }
    m_mutex.unlock ();

    bool done = false;
    Buffer *b = 0;
    qint64 l, m = 0;

    dispatch(PlayState::Playing);

    while (!done)
    {
        mutex()->lock ();
//        qDebug()<<QString();
//        qDebug()<<QString();
//        qDebug()<<QString("======= loop for [%1] ====").arg (Q_FUNC_INFO);

        if(m_pause != m_prev_pause) {
            if(m_pause) {
                qDebug()<<"suspend";
                m_output->suspend();
                mutex()->unlock();
                m_prev_pause = m_pause;
                continue;
            } else {
                qDebug()<<"resume";
                m_output->resume();
            }
            m_prev_pause = m_pause;
        }
        recycler()->mutex()->lock ();
        done = m_userStop || (m_finish && recycler()->empty());

        while (!done && (recycler()->empty() || m_pause)) {
            recycler()->cond()->wakeOne();
            mutex()->unlock();
            recycler()->cond()->wait(recycler()->mutex());
            mutex()->lock ();
            done = m_userStop || m_finish;
        }

        status();

        if (!b) {
            b = recycler()->next();
            if (b && b->rate)
                m_kbps = b->rate;
        }

        recycler()->cond()->wakeOne();
        recycler()->mutex()->unlock();
        mutex()->unlock();
        if (b) {
            mutex()->lock();
            if (m_useEq) {
                switch(m_audioParameters->format ()) {
                case AudioParameters::PCM_S16LE:
                    iir((void*) b->data, b->nbytes, m_audioParameters->channels ());
                    break;
                case AudioParameters::PCM_S24LE:
                    iir24((void*) b->data, b->nbytes, m_audioParameters->channels ());
                    break;
                case AudioParameters::PCM_S32LE:
                    iir32((void*) b->data, b->nbytes, m_audioParameters->channels ());
                    break;
                default:
                    ;
                }
            }
            mutex()->unlock();

            dispatchVisual(b);

            //TODO should use mutex lock ?
//            m_volume->changeVolume (b, mAudioParameters->channels (), mAudioParameters->format ());
            SoftVolume::instance ()->changeVolume (b, m_audioParameters->channels (), m_audioParameters->format ());
            if (m_muted)
                memset(b->data, 0, b->nbytes);
            l = 0;
            m = 0;
            while (l < b->nbytes && !m_pause && !m_prev_pause)
            {
                mutex()->lock();
                if(m_skip)
                {
                    m_skip = false;
                    m_output->reset();
                    mutex()->unlock();
                    break;
                }
                mutex()->unlock();
                m = m_output->writeAudio(b->data + l, b->nbytes - l);
//                qDebug()<<Q_FUNC_INFO<<"writeAudio ,write "<<m;
                if(m >= 0) {
                    m_totalWritten += m;
                    l+= m;
                } else {
                    break;
                }
            }
            if(m < 0)
                break;
        }
        mutex()->lock();
        //force buffer change
        recycler()->mutex()->lock ();
        recycler()->done();
        recycler()->mutex()->unlock();
        b = 0;
        mutex()->unlock();
    }
    mutex()->lock ();
    //write remaining data
    if(m_finish)
    {
        m_output->drain();
#ifdef Q_OS_WIN
        qDebug("OutputWriter: total written %I64d", m_totalWritten);
#else
        qDebug("OutputWriter: total written %lld", m_totalWritten);
#endif
    }
    dispatch(PlayState::Stopped);
    mutex()->unlock();
}

void OutputThread::dispatch(const PlayState &state)
{
    if (m_handler)
        m_handler->dispatch(state);
    if (state == PlayState::Stopped)
        clearVisual ();
}

void OutputThread::dispatch(qint64 elapsed, int bitrate, int frequency, int bits, int channels)
{
    if (m_handler)
         m_handler->dispatch(elapsed, bitrate, frequency, bits, channels);
}

void OutputThread::status()
{
    qint64 ct = m_totalWritten / m_bytesPerMillisecond - m_output->latency();

    if (ct < 0)
        ct = 0;
    if (ct > m_currentMilliseconds) {
        m_currentMilliseconds = ct;

//        qDebug()<<Q_FUNC_INFO<<"dispatch m_currentMilliseconds "<<m_currentMilliseconds
//                  <<" m_kbps "<<m_kbps<<" m_frequency "<<m_frequency<<" AudioParameters "<<m_audioParameters->parametersInfo ();

        dispatch(m_currentMilliseconds,
                 m_kbps,
                 m_frequency,
                 AudioParameters::sampleSize (m_audioParameters->format ()) *8,
                                              m_audioParameters->channels ());
    }
}

void OutputThread::clearVisual()
{
    if (m_visual) {
        m_visual->mutex ()->lock ();
        m_visual->clear ();
        m_visual->mutex ()->unlock ();
    }
}

void OutputThread::dispatchVisual(Buffer *buffer)
{
    if (!buffer || !m_visual)
        return;

    int sampleSize = m_audioParameters->sampleSize ();
    int samples = buffer->nbytes/sampleSize;
    int outSize = samples * 2;
    //increase buffer size
    if ((m_audioParameters->format () != AudioParameters::PCM_S16LE) && outSize > m_visBufferSize) {
        delete[] m_visBuffer;
        m_visBufferSize = outSize;
        m_visBuffer = new unsigned char [m_visBufferSize];
    }
    switch (m_audioParameters->format ()) {
    case AudioParameters::PCM_S8:
        s8_to_s16 ((qint8 *)buffer->data, (qint16 *) m_visBuffer, samples);
        break;
    case AudioParameters::PCM_S16LE:
        m_visBuffer = buffer->data;
        outSize = buffer->nbytes;
        break;
    case AudioParameters::PCM_S24LE:
        s24_to_s16 ((qint32 *)buffer->data, (qint16 *) m_visBuffer, samples);
        break;
    case AudioParameters::PCM_S32LE:
        s32_to_s16 ((qint32 *)buffer->data, (qint16 *) m_visBuffer, samples);
        break;
    default:
        return;
    }
    m_visual->mutex ()->lock ();
    m_visual->add (m_visBuffer, outSize, m_audioParameters->channels ());
    m_visual->mutex ()->unlock ();

    if (m_audioParameters->format () == AudioParameters::PCM_S16LE)
        m_visBuffer = 0;
}

void OutputThread::updateEQ()
{
    m_mutex.lock ();
    if (m_eq->enabled ()) {
        double preamp = m_eq->preamp ();
        int bands = m_eq->bands ();

        init_iir (m_audioParameters->sampleRate (), bands);

        set_preamp (0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
        set_preamp (1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);

        for (int i=0; i<bands; ++i) {
            double value = m_eq->value (i);
            set_gain (i, 0, 0.03*value + 0.000999999*value*value);
            set_gain (i, 1, 0.03*value + 0.000999999*value*value);
        }
    }
    m_useEq = m_eq->enabled ();
    m_mutex.unlock ();
}


} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
