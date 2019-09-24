/***************************************************************************
 *   Copyright (C) 2012-2019 by Ilya Kotov                                 *
 *   forkotov02@ya.ru                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "OutputThread.h"

#include <QtDebug>
#include <string.h>
#include <QCoreApplication>

#include "EqualizerMgr.h"

#include "AudioConverter.h"
#include "ChannelConverter.h"
#include "Buffer.h"
#include "StateHandler.h"
#include "SoftVolume.h"
#include "Recycler.h"

#include "IOutPut.h"
#include "QtMultimediaOutput.h"

extern "C" {
#include "equ/iir.h"
}

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

OutputThread::OutputThread(StateHandler *handle, Recycler *recycler, QObject* parent)
    : QThread(parent),
    m_handler(handle),
    m_recycler(recycler)
{
    m_frequency = 0;
    m_channels = 0;
    m_output = Q_NULLPTR;
    m_format = AudioParameters::AudioFormat::PCM_UNKNOWN;
    m_totalWritten = 0;
    m_currentMilliseconds = -1;
    m_bytesPerMillisecond = 0;
    m_user_stop = false;
    m_finish = false;
    m_kbps = 0;
    m_skip = false;
    m_pause = false;
    m_prev_pause = false;
    m_useEq = false;
    m_muted = false;
    m_format_converter = Q_NULLPTR;
    m_channel_converter = Q_NULLPTR;
    m_output_buf = Q_NULLPTR;
    m_output_size = 0;
    m_eq = EqualizerMgr::instance();
}

OutputThread::~OutputThread()
{
    stop();
    reset();

    if (m_output) {
        delete m_output;
        m_output = Q_NULLPTR;
    }
    if (m_format_converter) {
        delete m_format_converter;
        m_format_converter = Q_NULLPTR;
    }
    if (m_channel_converter) {
        delete m_channel_converter;
        m_channel_converter = Q_NULLPTR;
    }
    if (m_output_buf) {
        delete[] m_output_buf;
        m_output_buf = Q_NULLPTR;
    }
}

bool OutputThread::initialization(quint32 freq, ChannelMap map)
{
    reset();

    m_in_params = AudioParameters(freq, map, AudioParameters::PCM_FLOAT);
    if (!m_output) {
        m_output = new OutPut::QtMultimediaOutput::QtMultimediaOutput(this);
    }

    if (!m_output->initialize(freq, map, /*m_settings->outputFormat()*/AudioParameters::PCM_S16LE)) {
        qWarning()<<Q_FUNC_INFO<<"OutputWriter: unable to initialize output";
        delete m_output;
        m_output = nullptr;
        return false;
    }
    m_frequency = m_output->sampleRate();
    m_chan_map = m_output->channelMap();
    m_channels = m_chan_map.count();
    m_format = m_output->format();

    qDebug()<<Q_FUNC_INFO<<QString("OutputWriter: [QtMultimediaOutput] %1 ==> %2")
                                   .arg(m_in_params.parametersInfo())
                                   .arg(m_output->audioParameters().parametersInfo());


    if (!prepareConverters()) {
        qWarning()<<Q_FUNC_INFO<<"OutputWriter: unable to convert audio";
        delete m_output;
        m_output = nullptr;
        return false;
    }

    if (m_output_buf) {
        delete[] m_output_buf;
    }
    m_output_size = QMMP_BLOCK_FRAMES * m_channels * 4;
    m_output_buf = new unsigned char[m_output_size * m_output->sampleSize()];

    m_bytesPerMillisecond = m_frequency * m_channels * AudioParameters::sampleSize(m_format) / 1000;
    m_recycler->configure(m_in_params.sampleRate(), m_in_params.channels()); //calculate output buffer size
    updateEqSettings();
    clean_history();
    return true;
}

void OutputThread::togglePlayPause()
{
    qDebug()<<Q_FUNC_INFO<<" ----- ";

    m_pause = !m_pause;
    PlayState state = m_pause ? PlayState::Paused: PlayState::Playing;
    dispatch(state);

    m_recycler->mutex()->lock();
    m_recycler->cond()->wakeAll();
    m_recycler->mutex()->unlock();
}

bool OutputThread::isPaused() const
{
    return m_pause;
}

void OutputThread::stop()
{
    qDebug()<<Q_FUNC_INFO<<"===";

    m_user_stop = true;

    m_recycler->cond()->wakeAll();

    int loopOut = 3;
    do {
        qDebug()<<Q_FUNC_INFO<<"thread is runnging, loop remain "<<loopOut;
        loopOut--;
        qApp->processEvents();
    } while(this->isRunning() && loopOut > 0);

    if (this->isRunning()) {
        qDebug()<<Q_FUNC_INFO<<"wait thread";
        this->wait();
    }
    reset();
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
    m_mutex.lock();
    m_totalWritten = pos * m_bytesPerMillisecond;
    m_currentMilliseconds = -1;
    m_skip = isRunning() && reset;
    m_mutex.unlock();
}

AudioParameters OutputThread::audioParameters() const
{
    return AudioParameters(m_frequency, m_chan_map, AudioParameters::PCM_FLOAT);
}

int OutputThread::sampleSize() const
{
    return AudioParameters::sampleSize(m_format);
}

void OutputThread::dispatchVisual (Buffer *buffer)
{
    if (!buffer)
        return;

    //TODO
//    Visual::addAudio(buffer->data, buffer->samples, m_channels,
//                     m_totalWritten / m_bytesPerMillisecond, m_output->latency());
}

bool OutputThread::prepareConverters()
{
    if (m_format_converter) {
        delete m_format_converter;
        m_format_converter = Q_NULLPTR;
    }
    if (m_channel_converter) {
        delete m_channel_converter;
        m_channel_converter = Q_NULLPTR;
    }

    if (m_channels != m_output->channels()) {
        qWarning()<<Q_FUNC_INFO<<"OutputWriter: unsupported channel number";
        return false;
    }

    if (m_in_params.format() != m_format) {
        m_format_converter = new AudioConverter();
        m_format_converter->setFormat(m_format);
    }

    if (m_in_params.channelMap() != m_chan_map) {
        m_channel_converter = new ChannelConverter(m_chan_map);
        m_channel_converter->initialization(m_in_params.sampleRate(), m_in_params.channelMap());
    }
    return true;
}

void OutputThread::reset()
{
    m_frequency = 0;
    m_channels = 0;
    m_format = AudioParameters::AudioFormat::PCM_UNKNOWN;
    m_totalWritten = 0;
    m_currentMilliseconds = -1;
    m_bytesPerMillisecond = 0;
    m_user_stop = false;
    m_finish = false;
    m_kbps = 0;
    m_skip = false;
    m_pause = false;
    m_prev_pause = false;
    m_muted = false;
}

//void OutputWriter::startVisualization()
//{
//    foreach (Visual *visual, *Visual::visuals())
//    {
//        QMetaObject::invokeMethod(visual, "start", Qt::QueuedConnection);
//    }
//}

//void OutputWriter::stopVisualization()
//{
//    Visual::clearBuffer();
//    foreach (Visual *visual, *Visual::visuals())
//    {
//        QMetaObject::invokeMethod(visual, "stop", Qt::QueuedConnection);
//    }
//}

//void OutputWriter::dispatch(qint64 elapsed)
//{
//    if (m_handler)
//        m_handler->dispatchElapsed(elapsed);
//}

void OutputThread::dispatch(const PlayState &state)
{
    if (m_handler)
        m_handler->dispatch(state);
}

void OutputThread::dispatch(const AudioParameters &p)
{
//    if (m_handler)
//        m_handler->dispatch(p);
//TODO
}

void OutputThread::run()
{
    m_mutex.lock ();
    if (!m_bytesPerMillisecond) {
        qWarning()<<Q_FUNC_INFO<<"OutputWriter: invalid audio parameters";
        m_mutex.unlock ();
        return;
    }
    m_mutex.unlock ();

    bool done = false;
    Buffer *b = nullptr;
    quint64 l;
    qint64 m = 0;
    size_t output_at = 0;
    unsigned char *tmp = nullptr;

    dispatch(PlayState::Playing);
    dispatch(m_output->audioParameters());
//    startVisualization();

    while (!done) {
        m_mutex.lock ();
        if (m_pause != m_prev_pause) {
            if (m_pause) {
//                Visual::clearBuffer();
                m_output->suspend();
                m_mutex.unlock();
                m_prev_pause = m_pause;
                continue;
            } else {
                m_output->resume();
            }
            m_prev_pause = m_pause;
        }
        m_recycler->mutex()->lock ();
        done = m_user_stop || (m_finish && m_recycler->empty());

        while (!done && (m_recycler->empty() || m_pause)) {
            m_recycler->cond()->wakeOne();
            m_mutex.unlock();
            m_recycler->cond()->wait(m_recycler->mutex());
            m_mutex.lock ();
            done = m_user_stop || m_finish;
        }

        status();

        if (!b) {
            if((b = m_recycler->next())) {
                if (b->rate) {
                    m_kbps = b->rate;
                }
//                if(b->trackInfo)
//                    m_output->setTrackInfo(*b->trackInfo);
            }
        }

        m_recycler->cond()->wakeOne();
        m_recycler->mutex()->unlock();
        m_mutex.unlock();
        if (b) {
            m_mutex.lock();
            if (m_useEq) {
                iir(b->data, b->samples, m_channels);
            }
            m_mutex.unlock();

            dispatchVisual(b);

            SoftVolume::instance()->changeVolume(b, m_channels);

            if (m_muted) {
                memset(b->data, 0, b->size * sizeof(float));
            }
            if (m_channel_converter) {
                m_channel_converter->apply(b);
            }
            l = 0;
            m = 0;

            //increase buffer size if needed
            if (b->samples > m_output_size) {
                delete [] m_output_buf;
                m_output_size = b->samples;
                m_output_buf = new unsigned char[m_output_size * sampleSize()];
            }

            if (m_format_converter) {
                m_format_converter->fromFloat(b->data, m_output_buf, b->samples);
                tmp = m_output_buf;
            } else {
                tmp = (unsigned char*)b->data;
            }
            output_at = b->samples * m_output->sampleSize();

            while (l < output_at && !m_pause && !m_prev_pause) {
                m_mutex.lock();
                if(m_skip)
                {
                    m_skip = false;
//                    Visual::clearBuffer();
                    m_output->reset();
                    m_mutex.unlock();
                    break;
                }
                m_mutex.unlock();
                m = m_output->writeAudio(tmp + l, output_at - l);
                if (m >= 0) {
                    m_totalWritten += m;
                    l+= m;
                } else {
                    break;
                }
            }
            if(m < 0) {
                break;
            }
        }
        m_mutex.lock();
        //force buffer change
        m_recycler->mutex()->lock ();
        m_recycler->done();
        m_recycler->mutex()->unlock();
        b = nullptr;
        m_mutex.unlock();
    }
    m_mutex.lock();
    //write remaining data
    if(m_finish) {
        m_output->drain();
        qDebug()<<Q_FUNC_INFO<<"OutputWriter: total written" << m_totalWritten;
    }
    qDebug()<<Q_FUNC_INFO<<"Send finished";
    m_handler->sendFinished();
    dispatch(PlayState::Stopped);
//    stopVisualization();
    m_mutex.unlock();

    qDebug()<<">>>>>>>>>>>>>> FINISH "<<Q_FUNC_INFO<<" FINISH <<<<<<<<<<<<<<<<<<";
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
}

void OutputThread::updateEqSettings()
{
    m_mutex.lock();
    if (m_eq->enabled()) {
        double preamp = m_eq->preamp();
        int bands = m_eq->bands();

        init_iir(m_frequency, bands);

        set_preamp(0, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);
        set_preamp(1, 1.0 + 0.0932471 *preamp + 0.00279033 * preamp * preamp);

        for (int i=0; i<bands; ++i) {
            double value = m_eq->value (i);
            set_gain(i, 0, 0.03*value + 0.000999999*value*value);
            set_gain(i, 1, 0.03*value + 0.000999999*value*value);
        }
    }
    m_useEq = m_eq->enabled();
    m_mutex.unlock();
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
