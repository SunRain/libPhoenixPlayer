#include "DecodeThread.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QThread>
#include <QTimer>

#include "Backend/BaseMediaObject.h"

#include "PluginHost.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "PPCommon.h"
#include "PPSettings.h"
#include "LibPhoenixPlayerMain.h"
#include "MediaResource.h"

#include "Buffer.h"
#include "AudioParameters.h"
#include "StateHandler.h"
#include "Recycler.h"
#include "AudioConverter.h"
#include "ChannelConverter.h"
#include "Dithering.h"

#include "FFmpeg.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

using namespace Decoder::FFmpegDecoder;

const static int TRANSPORT_TIMEOUT = 5000; //ms

DecodeThread::DecodeThread(StateHandler *handle, Recycler *recycler, QObject *parent)
    : QThread(parent),
    m_handler(handle),
    m_recycler(recycler)
{
    m_output_buf = Q_NULLPTR;
    m_output_size = 0;
    m_bks = 0;
    m_sample_size = 0;
    //TODO use plugin later
    m_decoder = Q_NULLPTR;// new FFmpeg(this);
    m_replayGain = Q_NULLPTR;
    m_dithering = Q_NULLPTR;
    m_converter = new AudioConverter;

    reset();
}

DecodeThread::~DecodeThread()
{
    stop();
    reset();
    if (m_output_buf) {
        delete [] m_output_buf;
        m_output_buf = Q_NULLPTR;
    }

    if (m_converter) {
        delete  m_converter;
        m_converter = Q_NULLPTR;
    }
    //TODO use plugin later
    if (m_decoder) {
        m_decoder->deleteLater();
        m_decoder = Q_NULLPTR;
    }

}

bool DecodeThread::initialization(MediaResource *res, qint64 startTimeMS)
{
    reset();

    if (!res) {
        qWarning()<<Q_FUNC_INFO<<"nullptr media resource";
        return false;
    }

    m_resource = res;

    //TODO use plugin later
    if (!m_decoder) {
        m_decoder = new FFmpeg(this);
    }

    //TODO 判断媒体类型，也许可以添加一个单独的方法？
    QString uri = res->getUri ();

    qDebug()<<Q_FUNC_INFO<<"Change to uri "<<uri;

    //    if (res->mediaType () == Common::MediaTypeLocalFile) {
    //        //FIXME: memory leak?
    //        QFile *f = new QFile(uri, this);
    ////        m_decoder->setInputSource (qobject_cast<QIODevice *>(&f));
    //        m_decoder->setInputSource (f);
    //        m_decoder->setFileUri (uri);
    //    } else if (uri.toLower ().startsWith ("http://")
    //               || uri.toLower ().startsWith ("https://")
    //               || uri.toLower ().startsWith ("ftp://")) {
    ////        mPlayer->setMedia (QUrl(uri));
    //        //TODO 需要添加网络流支持
    //    }

    //FIXME this may make app crash, we should stop initialize;
    //    if (m_decoder->inputSource ()
    //            && !m_decoder->inputSource ()->isOpen ()
    //            && !m_decoder->inputSource ()->open (QIODevice::ReadOnly)) {
    //        qWarning()<<Q_FUNC_INFO<<QString("input error: %1").arg (m_decoder->inputSource ()->errorString ());
    //        if(m_handler->state() == PlayState::Stopped || m_handler->state() == PlayState::Buffering)
    //            m_handler->dispatch(PlayState::NormalError);
    //        return;
    //    }

    qDebug()<<Q_FUNC_INFO<<"Start initialize decoder";

    if (!m_decoder->initialize(res)) {
        qWarning()<<Q_FUNC_INFO<<"invalid file format";
        //TODO: 使用pluginloader切换到另外一个支持当前格式的decoder
        return false;
    }

    prepareEffects(m_decoder);

    if (m_decoder->durationInSeconds() <= 0) {
        m_seekTimeMS = -1;
    } else {
        if (startTimeMS > 0 && m_decoder->durationInSeconds() > startTimeMS/1000) {
            m_seekTimeMS = startTimeMS/1000;
        }
    }

    return true;
}

void DecodeThread::play()
{
    if (isRunning() || !m_decoder) {
        return;
    }
#ifdef Q_OS_WIN
    start(QThread::HighPriority);
#else
    start();
#endif
}

void DecodeThread::seek(qint64 millisecond)
{
    qDebug()<<Q_FUNC_INFO<<"=== seek to "<<millisecond;
    mutex()->lock ();
    m_seekTimeMS = millisecond;
    mutex()->unlock();
}

void DecodeThread::stop()
{
    qDebug()<<Q_FUNC_INFO<<"===";

    m_user_stop = true;
    m_recycler->cond()->wakeAll();

    int loopOut = 3;
    while (this->isRunning() && loopOut > 0) {
        qDebug()<<Q_FUNC_INFO<<"thread is runnging, loop remain "<<loopOut;
        loopOut--;
        qApp->processEvents();
    }

    if (this->isRunning()) {
        qDebug()<<Q_FUNC_INFO<<"wait thread";
        this->wait();
    }

//    clearDecoders();
    reset();
    qDeleteAll(m_effects);
    m_effects.clear();

    m_replayGain = Q_NULLPTR;
}

AudioParameters DecodeThread::audioParameters() const
{
    return m_ap;
}

bool DecodeThread::event(QEvent *event)
{
    return QObject::event(event);
}

void DecodeThread::run()
{
    this->mutex()->lock ();
    qint64 len = 0;
    int delay = 0;
    if (!m_decoder) {
        this->mutex()->unlock ();
        return;
    }
//    addOffset(); //offset
    this->mutex()->unlock();

    m_handler->dispatch(PlayState::Buffering);
    m_handler->dispatchDurationInMS(m_decoder->durationInSeconds() * 1000);
    m_handler->dispatch(PlayState::Playing);

    while (!m_done && !m_finish) {
        this->mutex()->lock ();
        //seek
        if (m_seekTimeMS >= 0) {
            m_decoder->setPositionMS(m_seekTimeMS);
            m_seekTimeMS = -1;
            m_recycler->mutex()->lock ();
            m_recycler->clear();
            m_recycler->mutex()->unlock ();
            m_output_at = 0;
        }
        //wait more data
        if (m_resource->pending()) {
            this->mutex()->unlock();
            msleep(5);
            delay += 5;
            m_done = m_user_stop.load();
            if (delay > TRANSPORT_TIMEOUT) {
                qWarning()<<Q_FUNC_INFO<<" unable to receive more data";
                m_done = true;
                m_handler->dispatch(PlayState::NormalError);
                break;
            }
            continue;
        } else {
            delay = 0;
            // decode
            len = m_decoder->read((m_output_buf + m_output_at), m_output_size - m_output_at);
        }

        if (len > 0) {
            m_bitrate = m_decoder->bitrate();
            m_output_at += len;
            flush();
        } else if (len == 0) {
//            if(m_next) //decoder can play next track without initialization
//            {
//                m_next = false;
//                qDebug("QmmpAudioEngine: switching to the next track");
//                StateHandler::instance()->sendFinished();
//                StateHandler::instance()->dispatch(Qmmp::Stopped); //fake stop/start cycle
//                StateHandler::instance()->dispatch(Qmmp::Buffering);
//                m_decoder->next();
//                StateHandler::instance()->dispatch(m_decoder->totalTime());
//                m_replayGain->setReplayGainInfo(m_decoder->replayGainInfo());
//                m_output->seek(0); //reset counter
//                addOffset(); //offset
//                StateHandler::instance()->dispatch(Qmmp::Playing);
//                mutex()->unlock();
//                continue;
//            }
//            else if(!m_decoders.isEmpty())
//            {
//                m_inputs.take(m_decoder)->deleteLater ();
//                delete m_decoder;
//                m_decoder = m_decoders.dequeue();
//                //m_seekTime = m_inputs.value(m_decoder)->offset();
//                flush(true);
//                //use current output if possible
//                prepareEffects(m_decoder);
//                if(m_ap == m_output->audioParameters())
//                {
//                    StateHandler::instance()->sendFinished();
//                    StateHandler::instance()->dispatch(Qmmp::Stopped); //fake stop/start cycle
//                    StateHandler::instance()->dispatch(Qmmp::Buffering);
//                    StateHandler::instance()->dispatch(m_decoder->totalTime());
//                    m_output->seek(0); //reset counter
//                    StateHandler::instance()->dispatch(Qmmp::Playing);
//                    mutex()->unlock();
//                    addOffset(); //offset
//                }
//                else
//                {
//                    finish();
//                    //wake up waiting threads
//                    mutex()->unlock();
//                    m_recycler->mutex()->lock ();
//                    m_recycler->cond()->wakeAll();
//                    m_recycler->mutex()->unlock();

//                    m_output->wait();
//                    delete m_output;
//                    m_output = createOutput();
//                    if(m_output)
//                    {
//                        m_output->start();
//                        StateHandler::instance()->dispatch(Qmmp::Playing);
//                        StateHandler::instance()->dispatch(m_decoder->totalTime());
//                        addOffset(); //offset
//                    }
//                }
//                if(!m_output)
//                    m_done = true;

//                continue;
//            }

            flush(true);
            m_recycler->mutex()->lock ();
            // end of stream
            while (!m_recycler->empty() && !m_user_stop) {
                m_recycler->cond()->wakeOne();
                this->mutex()->unlock();
                m_recycler->cond()->wait(m_recycler->mutex());
                this-> mutex()->lock ();
            }
            m_recycler->mutex()->unlock();

            //continue if new input was queued
            if (/*m_decoders.isEmpty() ||*/ m_user_stop) {
                m_done = true;
                m_finish = !m_user_stop;
            }
        } else {
            m_finish = true;
        }
        this->mutex()->unlock();
    }

//    clearDecoders();
    mutex()->lock ();
//    m_next = false;
    if (m_finish) {
        m_handler->decodeFinished();
    }
    m_recycler->cond()->wakeAll();

    this->mutex()->unlock();

    if (m_user_stop || (m_done && !m_finish)) {
        m_handler->requestStopOutput();
        m_recycler->cond()->wakeAll();
    }
    qDebug()<<">>>>>>>>>>>>>> FINISH "<<Q_FUNC_INFO<<" FINISH <<<<<<<<<<<<<<<<<<";
}

void DecodeThread::flush(bool final)
{
    ulong min = final ? 0 : m_bks;

    while ((!m_done && !m_finish) && m_output_at > min) {
        m_recycler->mutex()->lock ();

        while ((m_recycler->full() || m_recycler->blocked())
               && (!m_done && !m_finish)) {
            if (m_seekTimeMS > 0) {
                m_output_at = 0;
                m_recycler->mutex()->unlock ();
                return;
            }
            mutex()->unlock();
            m_recycler->cond()->wait(m_recycler->mutex());
            mutex()->lock ();
            m_done = m_user_stop.load();
        }

        if (m_user_stop || m_finish) {
            m_done = true;
        } else {
            m_output_at -= produceSound(m_output_buf, m_output_at, m_bitrate);
        }

        if (!m_recycler->empty()) {
            m_recycler->cond()->wakeOne();
        }

        m_recycler->mutex()->unlock();
    }
}

qint64 DecodeThread::produceSound(unsigned char *data, qint64 size, quint32 brate)
{
    Buffer *b = m_recycler->get();
//    b->trackInfo = m_trackInfo;
//    m_trackInfo.clear();
    size_t sz = size < m_bks ? size : m_bks;
    size_t samples = sz / m_sample_size;

    m_converter->toFloat(data, b->data, samples);

    b->samples = samples;
    b->rate = brate;
    foreach (auto effect, m_effects) {
        effect->apply(b);
    }
    memmove(data, data + sz, size - sz);
    m_recycler->add();
    return sz;
}

void DecodeThread::reset()
{
    m_done = false;
    m_finish = false;
    m_seekTimeMS = -1;
    m_output_at = 0;
    m_user_stop = false;
    m_bitrate = 0;
}

void DecodeThread::prepareEffects(Decoder::IDecoder *d)
{
    m_ap = d->audioParameters();
    //output buffer for decoder
    if (m_output_buf) {
        delete [] m_output_buf;
    }
    m_bks = QMMP_BLOCK_FRAMES * m_ap.frameSize(); //block size
    m_output_size = m_bks * 4;
    m_sample_size = m_ap.sampleSize();
    m_output_buf = new unsigned char[m_output_size];
    //audio converter
    m_converter->setFormat(m_ap.format());
    m_ap = AudioParameters(m_ap.sampleRate(), m_ap.channelMap(), AudioParameters::PCM_FLOAT);
#if 0
    //remove disabled and external effects
    foreach(Effect *e, m_effects)
    {
        if(!e->factory() || !Effect::isEnabled(e->factory()))
        {
            m_effects.removeAll(e);
            m_blockedEffects.removeAll(e);
        }
    }
#endif
//    m_replayGain = nullptr;
//    m_dithering = nullptr;
//        QList <AudioEffect *> tmp_effects = m_effects;
//        m_effects.clear();
    qDeleteAll(m_effects);
    m_effects.clear();

#if 0
    //replay gain
    {
        m_replayGain = new ReplayGain();
        m_replayGain->configure(m_ap.sampleRate(), m_ap.channelMap());
        m_effects << m_replayGain;
        m_replayGain->updateSettings(m_settings->replayGainMode(),
                                     m_settings->replayGainPreamp(),
                                     m_settings->replayGainDefaultGain(),
                                     m_settings->replayGainPreventClipping());
        m_replayGain->setReplayGainInfo(d->replayGainInfo());
    }
#endif
    //dithering
    {
        if (!m_dithering) {
            m_dithering = new Dithering();
        }
        m_dithering->initialization(m_ap.sampleRate(), m_ap.channelMap());
        m_effects << m_dithering;
    }
    //channel order converter
    if(m_ap.channelMap() != m_ap.channelMap().remaped())
    {
        m_effects << new ChannelConverter(m_ap.channelMap().remaped());
        m_effects.last()->initialization(m_ap.sampleRate(), m_ap.channelMap());
        m_ap = m_effects.last()->generateAudioParameters();
    }

#if 0

    foreach(EffectFactory *factory, Effect::enabledFactories())
    {
        Effect *effect = nullptr;
        foreach(Effect *e, tmp_effects) //find effect
        {
            if(e->factory() == factory)
                effect = e;
        }

        if(effect && (effect->audioParameters() != m_ap ||
                       m_blockedEffects.contains(effect))) //destroy effect which require restart
        {
            m_blockedEffects.removeAll(effect);
            tmp_effects.removeAll(effect);
            delete effect;
            effect = nullptr;
        }
        if(!effect)
        {
            effect = Effect::create(factory);
            effect->configure(m_ap.sampleRate(), m_ap.channelMap());
            if (m_ap != effect->audioParameters())
            {
                m_blockedEffects << effect; //list of effects which require restart
                m_ap = effect->audioParameters();
            }
        }
        m_effects << effect;
        tmp_effects.removeAll(effect);
    }
#endif

    m_dithering->setFormats(d->audioParameters().format(), m_ap.format());

}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
