#include "OutputThread.h"

#include <QDebug>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioOutput>

//#include "OutPut/IOutPut.h"
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

#include "PhoenixBackend_global.h"
//#include "OutPut/OutPutHost.h"

extern "C" {
#include "equ/iir.h"
}

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

OutputThread::OutputThread(RingBuffer *ring, BaseVisual *v, QObject *parent)
    : QThread(parent)
    , m_output(nullptr)
    , m_device(nullptr)
    , m_eq(EqualizerMgr::instance ())
//    , m_handler(StateHandler::instance ())
    , m_visual(v)
    , m_ring(ring)
    , m_visBuffer(nullptr)
    , m_userStop(false)
    , m_pause(false)
//    , m_prev_pause(false)
//    , m_finish(false)
    , m_useEq(false)
    , m_muted(false)
//    , m_skip(false)
//    , m_kbps(0)
//    , m_frequency(0)
//    , m_bytesPerMillisecond(-1)
//    , m_totalWritten(0)
//    , m_currentMilliseconds(0)
    , m_visBufferSize(0)
{
//    m_audioParameters = AudioParameters();
    connect (m_eq, &EqualizerMgr::changed, this, &OutputThread::updateEQ);
}

OutputThread::~OutputThread()
{

}

bool OutputThread::initialize(const AudioParameters &para)
{
    m_audioParameters = para;

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
//    format.setSampleType(QAudioFormat::SignedInt);
//    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleSize(para.sampleSize ());

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
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

}

void OutputThread::reset()
{
//    m_frequency = 0;
//    m_totalWritten = 0;
//    m_currentMilliseconds = -1;
//    m_bytesPerMillisecond = 0;
//    m_userStop = false;
//    m_finish = false;
//    m_kbps = 0;
//    m_skip = false;
    m_pause = false;
//    m_prev_pause = false;
    m_useEq = false;
}

void OutputThread::stop()
{
    m_userStop = true;
}

void OutputThread::setMuted(bool muted)
{

}

void OutputThread::finish()
{

}

void OutputThread::seek(qint64 pos, bool reset)
{

}

void OutputThread::run()
{
    Buffer buffer(m_ring->bufferSize ());
    bool done = false;
    while (!done) {
        if (m_ring->empty ()) {
            continue;
        }
        if (!m_ring->pop (&buffer)) {
            continue;
        }
        if (m_muted) {
            continue;
        }
        done = m_userStop;
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
//            if (m_muted)
//                memset(buffer.data, 0, b.nbytes);
            int write = 0;
            do {
                int i = m_device->write (((char*)buffer.data + write),
                                         qMin((ulong)m_ring->bufferSize (), buffer.nbytes));
                write += i;
                buffer.nbytes -= i;
    //            qDebug()<<"write size "<<i<<" all write "<<write<<" left "<<b.nbytes;
//                qApp->processEvents ();
            } while (buffer.nbytes > 0);
        }
    }

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



} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
