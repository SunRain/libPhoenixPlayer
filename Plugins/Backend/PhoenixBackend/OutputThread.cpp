#include "OutputThread.h"

#include "OutPut/IOutPut.h"
#include "OutPut/OutPutHost.h"

#include "AudioParameters.h"
#include "Buffer.h"
#include "RingBuffer.h"

#include "EqualizerMgr.h"
#include "LibPhoenixPlayerMain.h"

#include "Backend/BaseVolume.h"
#include "Backend/SoftVolume.h"
#include "Backend/BaseVisual.h"

#include "PluginLoader.h"
#include "PhoenixBackend_global.h"

#include "StateHandler.h"
#include "AudioConverter.h"
#include "ChannelConverter.h"
#include "BufferQueue.h"

extern "C" {
#include "equ/iir.h"
}

using namespace PhoenixPlayer::OutPut;
using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;


OutputThread::OutputThread(StateHandler *handler,
                           BufferQueue *queue,
//                           QList<AudioEffect *> *list,
                           QObject *parent)
    : QThread (parent),
    m_handler(handler),
    m_bufferQueue(queue)
//    m_effectList(list)
{
    m_outputHost = phoenixPlayerLib->pluginLoader()->curOutPutHost();
    if (m_outputHost) {
        if (m_outputHost->isValid()) {
            m_output = m_outputHost->instance<IOutPut>();
        }
    }

    m_eq = EqualizerMgr::instance();
    connect (m_eq, &EqualizerMgr::changed, this, &OutputThread::updateEQ);
}

OutputThread::~OutputThread()
{

}

bool OutputThread::initialization(quint32 sampleRate, const QList<PhoenixPlayer::AudioParameters::ChannelPosition> &list)
{
    m_in_params = AudioParameters(sampleRate, list, AudioParameters::PCM_FLOAT);

    if (!m_output && m_outputHost) {
        if (m_outputHost->isValid())
            m_output = m_outputHost->instance<IOutPut>();
    }
    if (!m_output) {
        qCritical()<<Q_FUNC_INFO<<"Can't find output!!";
        return false;
    }

    if (!m_output->initialize(sampleRate, list, AudioParameters::PCM_FLOAT)) {
        qCritical()<<Q_FUNC_INFO<<"Can't init output";
        if (m_outputHost) {
            if (!m_outputHost->unLoad())
                m_outputHost->forceUnload();
        }
        m_output = nullptr;
        return false;
    }
    this->m_sampleRate = m_output->sampleRate();
    this->m_channels = m_output->channels();
    this->m_format = m_output->format();

    qDebug()<<Q_FUNC_INFO<<" ["<<m_in_params.parametersInfo()
             <<"] ==> ["<<m_output->audioParameters().parametersInfo()<<"]";

    if(m_format_converter) {
        delete m_format_converter;
        m_format_converter = nullptr;
    }
    if(m_channel_converter) {
        delete m_channel_converter;
        m_channel_converter = nullptr;
    }

    if (m_in_params.format() != m_format) {
        m_format_converter = new AudioConverter();
        m_format_converter->setFormat(m_format);
    }
    if (m_in_params.channels() != m_channels) {
        m_channel_converter = new ChannelConverter(m_channels);
        m_channel_converter->initialization(m_in_params.sampleRate(), m_in_params.channels());

    }

    if(m_output_buf) {
        delete [] m_output_buf;
        m_output_buf = Q_NULLPTR;

    }
    m_output_size = Buffer::BUFFER_PERIOD * m_channels.size() * 4;
    m_output_buf = new unsigned char[m_output_size * m_output->sampleSize()];

    m_bytesPerMillisecond = m_sampleRate
                            * m_channels.size()
                            * AudioParameters::sampleSize(m_format) / 1000;

    updateEQ();
    clean_history();
    return true;
}

bool OutputThread::isPaused()
{
    return m_paused;
}

void OutputThread::togglePlayPause()
{
    m_mutex.lock();
    m_paused = !m_paused;
    m_mutex.unlock();

    PlayState state = m_paused ? PlayState::Paused : PlayState::Playing;
    m_handler->dispatch(state);
    if (!m_paused) {
        m_pauseWait.wakeAll();
    }
}

void OutputThread::reset()
{
    m_totalWritten = 0;
    m_currentMilliseconds = 0;
    m_bytesPerMillisecond = 0;
    m_user_stop = false;
    m_finish = false;
    //    m_kbps = 0;
//    m_skip = false;
    m_paused = false;
    m_muted = false;
//    m_prev_pause = false;
    //    m_useEq = false;
}

void OutputThread::stop()
{
    QMutexLocker locker(&m_mutex);
    if (m_paused) {
        m_pauseWait.wakeAll();
        m_paused = !m_paused;
    }
    m_user_stop = true;
}

void OutputThread::setMuted(bool muted)
{
    m_mutex.lock();
    m_muted = muted;
    m_mutex.unlock();
}

void OutputThread::finish()
{
    m_finish = true;
}

bool OutputThread::event(QEvent *event)
{
    return QThread::event(event);
}

void OutputThread::run()
{
    m_mutex.lock();
    if (!m_bytesPerMillisecond) {
        qWarning()<<Q_FUNC_INFO<<"Invalid m_bytesPerMillisecond";
        m_mutex.unlock();
        return;
    }
    m_mutex.unlock();

    m_handler->dispatch(PlayState::Playing);

    uchar *tmp = Q_NULLPTR;
    size_t output_at = 0;

    bool outputTerminate = false;

    while (true) {

        QMutexLocker locker(&m_mutex);

        if (m_paused) {
            m_pauseWait.wait(locker.mutex());
        }

        if (m_user_stop) break;

        m_bufferQueue->mutex()->lock();
        if (m_bufferQueue->isEmpty()) {
            m_bufferQueue->waitIn()->wait(m_bufferQueue->mutex());
        }
        m_bufferQueue->mutex()->unlock();

        if (m_user_stop) break;

        status();

        Buffer *b = m_bufferQueue->dequeue();

        if (m_useEq) {
            iir(b->data, b->samples, m_channels.count());
        }

        SoftVolume::instance()->changeVolume(b, m_channels.count());

        if (m_muted) {
            memset(b->data, 0, b->size * sizeof(float));
        }
        if (m_channel_converter) {
            m_channel_converter->apply(b);
        }

        //increase buffer size if needed
        if(b->samples > m_output_size) {
            delete [] m_output_buf;
            m_output_size = b->samples;
            m_output_buf = new unsigned char[m_output_size * AudioParameters::sampleSize(m_format)];
        }

        if(m_format_converter) {
            m_format_converter->fromFloat(b->data, m_output_buf, b->samples);
            tmp = m_output_buf;
        } else {
            tmp = (unsigned char*)b->data;
        }

        output_at = b->samples * m_output->sampleSize();
        {
            qint64 pos = 0;
            do {
                qint64 len = m_output->writeAudio(tmp + pos, output_at - pos);
                if (len >= 0) {
                    m_totalWritten += len;
                    pos += len;
                } else {
                    outputTerminate = true;
                    break;
                }
            } while (true);
            if (outputTerminate) break;
        }


    }




}

void OutputThread::updateEQ()
{
    if (m_eq->enabled()) {
        double preamp = m_eq->preamp();
        int bands = m_eq->bands();

        init_iir(m_sampleRate, bands);

        set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
        set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);

        for (int i=0; i<bands; ++i) {
            double value = m_eq->value (i);
            set_gain(i, 0, 0.03*value + 0.000999999*value*value);
            set_gain(i, 1, 0.03*value + 0.000999999*value*value);
        }
    }
    m_mutex.lock();
    m_useEq = m_eq->enabled();
    m_mutex.unlock();
}

void OutputThread::status()
{
    qint64 ct = m_totalWritten / m_bytesPerMillisecond - m_output->latency();

    if (ct < 0) {
        ct = 0;
    }

    if (ct > m_currentMilliseconds) {
        m_currentMilliseconds = ct;
        m_handler->dispatchElapsed(m_currentMilliseconds);
    }
    qDebug()<<Q_FUNC_INFO<<">>>>>>> m_currentMilliseconds "<<m_currentMilliseconds;
}
