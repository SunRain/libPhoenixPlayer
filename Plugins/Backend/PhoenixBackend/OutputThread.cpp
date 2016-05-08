#include "OutputThread.h"

#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>

#include "OutPut/IOutPut.h"
#include "AudioParameters.h"
//#include "Recycler.h"
#include "Buffer.h"
#include "RingBuffer.h"

#include "SingletonPointer.h"
#include "EqualizerMgr.h"
#include "LibPhoenixPlayerMain.h"

#include "Backend/BaseVolume.h"
#include "Backend/SoftVolume.h"
#include "Backend/BaseVisual.h"

#include "PluginLoader.h"
#include "PhoenixBackend_global.h"
#include "OutPut/OutPutHost.h"

#include "StateHandler.h"

extern "C" {
#include "equ/iir.h"
}

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

using namespace OutPut;

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

OutputThread::OutputThread(RingBuffer *ring, BaseVisual *v, QObject *parent)
    : QThread(parent)
    , m_output(nullptr)
#if 0
    , m_device(nullptr)
#endif
    , m_eq(EqualizerMgr::instance ())
//    , m_handler(StateHandler::instance ())
    , m_visual(v)
    , m_ring(ring)
    , m_visBuffer(nullptr)
    , m_userStop(false)
    , m_pause(false)
    , m_prev_pause(false)
    , m_finish(false)
    , m_useEq(false)
    , m_muted(false)
    , m_skip(false)
//    , m_kbps(0)
//    , m_frequency(0)
    , m_bytesPerMillisecond(0)
    , m_totalWritten(0)
    , m_currentMilliseconds(0)
    , m_visBufferSize(0)
{
    m_outputHost = phoenixPlayerLib->pluginLoader ()->curOutPutHost ();
    if (m_outputHost) {
        if (m_outputHost->isValid ())
            m_output = m_outputHost->instance<IOutPut>();
    }
    m_handler = StateHandler::instance();

    connect (m_eq, &EqualizerMgr::changed, this, &OutputThread::updateEQ);
}

OutputThread::~OutputThread()
{

}

bool OutputThread::initialize(const AudioParameters &para)
{
    m_audioParameters = para;

#if 0
    if (m_device) {
        m_device->deleteLater ();
        m_device = nullptr;
    }
    if (m_output) {
        m_output->deleteLater ();
        m_output = nullptr;
    }

    QAudioFormat format;
    format.setSampleRate (para.sampleRate ());
    format.setChannelCount (para.channels ());
    format.setCodec ("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleSize(16);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    qDebug()<<Q_FUNC_INFO<<" preferredFormat "<<info.preferredFormat ();
    qDebug()<<Q_FUNC_INFO<<" supportedByteOrders "<<info.supportedByteOrders ();
    qDebug()<<Q_FUNC_INFO<<" supportedSampleRates "<<info.supportedSampleRates ();
    qDebug()<<Q_FUNC_INFO<<" supportedSampleSizes "<<info.supportedSampleSizes ();
    qDebug()<<Q_FUNC_INFO<<" supportedSampleTypes "<<info.supportedSampleTypes ();


    if (!info.isFormatSupported(format)) {
        qDebug()<<Q_FUNC_INFO<<"Raw audio format not supported by backend, cannot play audio.";
        return false;
    }
    m_output = new QAudioOutput(format, this);
    m_device = m_output->start ();
    QAudio::Error error = m_output->error ();
    if (error != QAudio::NoError) {
        qDebug()<<Q_FUNC_INFO<<"Open output error, code "<<error;
        return false;
    }
#endif
    if (!m_output && m_outputHost) {
        if (m_outputHost->isValid ())
            m_output = m_outputHost->instance<IOutPut>();
    }
    if (!m_output) {
        qCritical()<<Q_FUNC_INFO<<"Can't find output!!";
        return false;
    }

    if (!m_output->initialize (para)) {
        qCritical()<<Q_FUNC_INFO<<"Can't init output";
//        m_pluginHost = m_pluginLoader->getCurrentPluginHost (Common::PluginOutPut);
//        m_pluginHost->unLoad ();
        if (m_outputHost) {
            if (!m_outputHost->unLoad ())
                m_outputHost->forceUnload ();
        }
        m_output = nullptr;
        return false;
    }

    reset ();

    m_bytesPerMillisecond = para.sampleRate () * para.channels () * para.sampleSize () /1000;

    if (m_visBuffer) {
        delete[] m_visBuffer;
    }
    m_visBufferSize = Buffer::BUFFER_PERIOD * 2 * para.channels (); //16-bit samples
//    if (m_audioParameters->format () != AudioParameters::PCM_S16LE)
//        m_visBuffer = new unsigned char [m_visBufferSize];
    if (m_audioParameters.format () != AudioParameters::PCM_S16LE)
        m_visBuffer = new unsigned char [m_visBufferSize];

    updateEQ ();
    return true;
}

bool OutputThread::isPaused()
{
    return m_pause;
}

void OutputThread::togglePlayPause()
{
//    qDebug()<<Q_FUNC_INFO<<"==== change ======== ";
    m_mutex.lock ();
    m_pause = !m_pause;
//    qDebug()<<Q_FUNC_INFO<<"==== change2 ======== ";
    m_mutex.unlock ();
//    m_ring->cond ()->wakeAll ();
    m_wait.wakeAll ();
    PlayState state = m_pause ? PlayState::Paused : PlayState::Playing;
    m_handler->dispatch(state);
}

void OutputThread::reset()
{
//    m_frequency = 0;
    m_totalWritten = 0;
    m_currentMilliseconds = 0;
    m_bytesPerMillisecond = 0;
    m_userStop = false;
    m_finish = false;
//    m_kbps = 0;
    m_skip = false;
    m_pause = false;
    m_prev_pause = false;
    m_useEq = false;
}

void OutputThread::stop()
{
    m_userStop = true;
//    reset ();
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
    qDebug()<<Q_FUNC_INFO<<"seek to "<<pos<<" should reset "<<reset;
    m_totalWritten = pos * m_bytesPerMillisecond;
    m_currentMilliseconds = 0;
    m_skip = this->isRunning () && reset;
    qDebug()<<Q_FUNC_INFO<<" m_skip "<<m_skip;
}

void OutputThread::run()
{
//    qDebug()<<">>>>>>>>>>>>>>>>>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<<<<<<<<<<<";
    Buffer buffer(m_ring->bufferSize ());
    bool done = false;
    bool poped = true;
    m_handler->dispatch(PlayState::Playing);
    while (!done) {
//        if (m_pause) {
//            continue;
//        }
//        if (m_ring->empty ()) {
//            qDebug()<<"###### wait";
//            continue;
//        }
//        if (!m_ring->pop (&buffer)) {
//            continue;
//        }
//        qDebug()<<" xxxxxxxxxxxxxxxxxx "<<Q_FUNC_INFO<<" xxxxxxxxxx";
        m_ring->mutex ()->lock ();
        poped = m_ring->pop (&buffer);
        if (m_ring->empty () || !poped) {
            m_ring->fullCond ()->wakeAll ();
            m_ring->emptyCond ()->wait (m_ring->mutex ());
//            continue;
        }
        if (poped) {
            m_ring->fullCond ()->wakeAll ();
        }
        m_ring->mutex ()->unlock ();

        if (m_muted) {
            continue;
        }
//        qDebug()<<"########### step 1";

        //check if pause or resume when start a new wirte loop
        m_mutex.lock ();
        if (m_pause != m_prev_pause) {
            if (m_pause) {
                m_output->suspend ();
                m_mutex.unlock ();
                m_prev_pause = m_pause;
                continue;
            } else {
                m_output->resume ();
            }
            m_prev_pause = m_pause;
        }

//        m_ring->mutex ()->lock ();

        done = m_userStop || m_finish;

        //if pause, loop to wait for resume
        while (!done && m_pause) {
//            QThread::yieldCurrentThread ();
//            qDebug()<<"loop in pause";
//            m_mutex.lock ();
//            m_mutex.unlock ();
//            m_ring->cond ()->wait (m_ring->mutex ());
            m_wait.wait (&m_mutex);
//            m_mutex.lock ();
            done = m_userStop || m_finish;
//            m_mutex.unlock ();
//            continue;
        }
//        m_ring->mutex ()->unlock ();

        status ();

        m_mutex.unlock ();

        if (buffer.nbytes >0 && buffer.data) {
            if (m_useEq) {
                switch(m_audioParameters.format ()) {
                case AudioParameters::PCM_S16LE:
                    iir((void*) buffer.data, buffer.nbytes, m_audioParameters.channels ());
                    break;
                case AudioParameters::PCM_S24LE:
                    iir24((void*) buffer.data, buffer.nbytes, m_audioParameters.channels ());
                    break;
                case AudioParameters::PCM_S32LE:
                    iir32((void*) buffer.data, buffer.nbytes, m_audioParameters.channels ());
                    break;
                default:
                    ;
                }
            }
            SoftVolume::instance ()->changeVolume (&buffer,
                                                   m_audioParameters.channels (), m_audioParameters.format ());
            if (m_muted)
                memset(buffer.data, 0, buffer.nbytes);

            //change audio to 16bit
            switch (m_audioParameters.format ()) {
            case AudioParameters::PCM_S8: {
                unsigned char *out = new unsigned char[buffer.nbytes*2];
                s8_to_s16((qint8 *)buffer.data, (qint16 *) out, buffer.nbytes);
                delete [] buffer.data;
                buffer.data = out;
                buffer.nbytes <<= 1;
                break;
            }
            case AudioParameters::PCM_S24LE: {
                s24_to_s16((qint32 *)buffer.data, (qint16 *)buffer.data, buffer.nbytes >> 2);
                buffer.nbytes >>= 1;
                break;
            }
            case AudioParameters::PCM_S32LE: {
                s32_to_s16((qint32 *)buffer.data, (qint16 *)buffer.data, buffer.nbytes >> 2);
                buffer.nbytes >>= 1;
                break;
            }
            default:
                break;
            }
            m_audioParameters.setFormat (AudioParameters::PCM_S16LE);

            //write to output
            int write = 0;
            while (buffer.nbytes > 0 /*&& !m_pause && !m_prev_pause*/) {
                if (m_skip) {
                    m_skip = false;
                    m_output->reset ();
                    break;
                }
                if (m_muted) break;

                //when pause in write loop
//                m_ring->mutex ()->lock ();
                m_mutex.lock ();
                if (m_pause) {
//                    continue;
//                    m_ring->cond ()->wait (m_ring->mutex ());
                    m_wait.wait (&m_mutex);
                }
//                m_ring->mutex ()->unlock ();
                m_mutex.unlock ();
//                qDebug()<<"##### write to output";
#if 0
                int i = m_device->write (((char*)buffer.data + write),
                                         qMin((ulong)m_ring->bufferSize (), buffer.nbytes));
#else
                int i = m_output->writeAudio ((buffer.data + write),
                                              qMin((ulong)m_ring->bufferSize (), buffer.nbytes));
#endif
                write += i;
                m_totalWritten += i;
                buffer.nbytes -= i;
//                qDebug()<<"write size "<<i<<" all write "<<write<<" left "<<buffer.nbytes;
//                qApp->processEvents ();
            }
        }
    }
    //write remain data
    if (m_finish) {
        m_output->drain ();
    }

    m_handler->dispatch(PlayState::Stopped);
}

void OutputThread::updateEQ()
{
    if (m_eq->enabled ()) {
        double preamp = m_eq->preamp ();
        int bands = m_eq->bands ();

        init_iir (m_audioParameters.sampleRate (), bands);

        set_preamp (0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
        set_preamp (1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);

        for (int i=0; i<bands; ++i) {
            double value = m_eq->value (i);
            set_gain (i, 0, 0.03*value + 0.000999999*value*value);
            set_gain (i, 1, 0.03*value + 0.000999999*value*value);
        }
    }
    m_useEq = m_eq->enabled ();
}

void OutputThread::status()
{
    qint64 ct = m_totalWritten / m_bytesPerMillisecond - m_output->latency();

    if (ct < 0)
        ct = 0;

    if (ct > m_currentMilliseconds) {
        m_currentMilliseconds = ct;
        m_handler->dispatchElapsed(m_currentMilliseconds);
    }
//    qDebug()<<Q_FUNC_INFO<<">>>>>>> m_currentMilliseconds "<<m_currentMilliseconds;
}



} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
