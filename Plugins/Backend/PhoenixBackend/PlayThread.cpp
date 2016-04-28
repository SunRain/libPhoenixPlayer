#include "PlayThread.h"

#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QThread>

#include "OutPut/IOutPut.h"
#include "Decoder/IDecoder.h"
#include "Decoder/DecoderHost.h"

#include "Backend/BaseMediaObject.h"

#include "AudioParameters.h"
#include "PluginHost.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "Common.h"
#include "Buffer.h"
#include "Settings.h"
#include "LibPhoenixPlayerMain.h"

#include "OutputThread.h"
#include "StateHandler.h"
#include "Recycler.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

using namespace Decoder;

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

PlayThread::PlayThread(QObject *parent, BaseVisual *v)
    :QThread(parent)
    ,m_visual(v)
{
    m_pluginLoader = phoenixPlayerLib->pluginLoader ();
    m_settings = phoenixPlayerLib->settings ();
    m_handler = StateHandler::instance ();

    m_decoderLibs = m_settings->decoderLibraries ();
    if (m_decoderLibs.isEmpty ())
        m_decoderLibs = m_pluginLoader->pluginLibraries (Common::PluginDecoder);

    qDebug()<<Q_FUNC_INFO<<" m_decoderLibs "<<m_decoderLibs
              <<"m_decoderLibs size "<<m_decoderLibs.size ()
           <<" all decoderLibs "<<m_pluginLoader->pluginLibraries (Common::PluginDecoder);

    m_output_buf = 0;
    m_output_size = 0;
    m_bks = 0;
    m_startPos = -1;
    m_decoder = nullptr;
    m_outputThread = nullptr;
    m_audioParameters = nullptr;
    
    //FIXME where should we put the output convert flag?
    m_convertAudioParameters = nullptr;
    m_use16BitOutputConvert = false;
    
    m_muted = false;
    reset();
}

PlayThread::~PlayThread()
{
    stop();
//    reset();
    if(m_output_buf)
        delete [] m_output_buf;
    m_output_buf = nullptr;
    if (m_visual)
        m_visual = nullptr;
}

QMutex *PlayThread::mutex()
{
    return &m_mutex;
}

bool PlayThread::play()
{
    if (this->isRunning () || !m_decoder || (m_outputThread && m_outputThread->isRunning ())) {
        qDebug()<<Q_FUNC_INFO<<"Can't start play due to"
               <<QString("current thread running [%1], decoder [%2] || m_ouputThread && m_ouputThread->isRunning [%3]")
                 .arg (this->isRunning ()).arg ((m_decoder == nullptr)).arg ((m_outputThread && m_outputThread->isRunning ()));
        return false;
    }
    if (m_outputThread) {
        m_outputThread->mutex ()->lock ();
        m_outputThread->finish ();
        m_outputThread->stop ();
        m_outputThread->mutex ()->unlock ();
        m_outputThread->quit ();
        m_outputThread->wait ();
//        m_outputThread->deleteLater ();
//        m_outputThread = nullptr;
    }
    if (m_audioParameters) {
        m_audioParameters->deleteLater ();
        m_audioParameters = nullptr;
    }
    qDebug()<<Q_FUNC_INFO<<"m_decoder->audioParameters "<<m_decoder->audioParameters ()->parametersInfo ();

    m_audioParameters = new AudioParameters(m_decoder->audioParameters (), this);

    //FIXME we should not destruct && construct every time
//    m_outputThread = createOutput ();
    createOutput ();
    if (!m_outputThread) {
        qDebug()<<Q_FUNC_INFO<<"cant create output thread";
        return false;
    }
    start ();
    return true;
}

void PlayThread::seek(qint64 millisecond)
{
    qDebug()<<Q_FUNC_INFO<<"=== seek to "<<millisecond;
    if (m_outputThread && m_outputThread->isRunning()) {
        m_outputThread->mutex()->lock ();
        m_outputThread->seek(millisecond, true);
        m_outputThread->mutex()->unlock();
        if (isRunning()) {
            mutex()->lock ();
            m_seekTime = millisecond;
            mutex()->unlock();
        }
    }
}

void PlayThread::stop()
{
    qDebug()<<Q_FUNC_INFO<<"===";
    m_mutex.lock ();
    m_user_stop = true;
    m_mutex.unlock ();

    if (m_outputThread)
        m_outputThread->recycler ()->cond ()->wakeAll ();
    if (this->isRunning ()) {
        qDebug()<<Q_FUNC_INFO<<"Wait for PlayThread finish";
        this->quit ();
        this->wait ();
    }
    if (m_outputThread) {
        m_outputThread->mutex ()->lock ();
        m_outputThread->finish ();
        m_outputThread->stop ();
        m_outputThread->mutex ()->unlock ();

        m_outputThread->quit ();
//        m_outputThread->wait ();
    }
    // wake up threads
    qDebug()<<Q_FUNC_INFO<<"wake up threads";
    if (m_outputThread) {
        m_outputThread->mutex ()->lock ();
        m_outputThread->recycler ()->cond ()->wakeAll ();
        m_outputThread->mutex ()->unlock ();
        if (m_outputThread->isRunning ()) {
            qDebug()<<Q_FUNC_INFO<<"wait m_ouputThread to finish";
            m_outputThread->quit ();
            m_outputThread->wait ();
//            m_ouputThread->deleteLater ();
//            m_ouputThread = nullptr;
        }
    }
    //FIXME should clear decoder?

    //reset values
    reset ();

    //TODO should remove effects when stop ?
}

void PlayThread::togglePlayPause()
{
    if (m_outputThread) {
        m_outputThread->togglePlayPause ();
        m_outputThread->recycler ()->mutex ()->lock ();
        m_outputThread->recycler ()->cond ()->wakeAll ();
        m_outputThread->recycler ()->mutex ()->unlock ();
    }
}

void PlayThread::setMuted(bool muted)
{
    m_muted = muted;
    if (m_outputThread) {
        m_outputThread->mutex ()->lock ();
        m_outputThread->setMuted (muted);
        m_outputThread->mutex ()->unlock ();
    }
}

void PlayThread::changeMedia(BaseMediaObject *obj, quint64 startSec)
{
    stop ();

    if (m_decoder) {
        //TODO 判断当前decoder是否支持解析当前的media
    } else {
        //TODO 使用PluginLoader装载不同的Decoder，需要添加Decoder是否支持当前媒体的接口
        foreach (QString s, m_decoderLibs) {
            m_decoderHost = new DecoderHost(s);
            if (m_decoderHost->isValid ()) { //TODO 需要判断当前decoder/host是否支持当前媒体的解码
                m_decoder = m_decoderHost->instance<IDecoder>();
                if (!m_decoder) {
                    if (!m_decoderHost->unLoad ()) {
                        m_decoderHost->forceUnload ();
                    }
                    m_decoder = nullptr;
                    m_decoderHost->deleteLater ();
                    m_decoderHost = nullptr;
                    continue;
                }
                break;
            }
        }
    }

    if (!m_decoder) {
        qCritical()<<Q_FUNC_INFO<<"No decoder found!!";
        return;
    }
    //TODO 判断媒体类型，也许可以添加一个单独的方法？
    QString uri;
    if (!obj->filePath ().isEmpty () && !obj->fileName ().isEmpty ()) {
        uri = QString("%1/%2").arg (obj->filePath ()).arg (obj->fileName ());
    } else {
        uri = obj->filePath ().isEmpty () ? obj->fileName () : obj->filePath ();
    }

    qDebug()<<Q_FUNC_INFO<<"Change to uri "<<uri;

    if (obj->mediaType () == Common::MediaTypeLocalFile) {
        //FIXME: memory leak?
        QFile *f = new QFile(uri, this);
//        m_decoder->setInputSource (qobject_cast<QIODevice *>(&f));
        m_decoder->setInputSource (f);
        m_decoder->setFileUri (uri);
    } else if (uri.toLower ().startsWith ("http://")
               || uri.toLower ().startsWith ("https://")
               || uri.toLower ().startsWith ("ftp://")) {
//        mPlayer->setMedia (QUrl(uri));
        //TODO 需要添加网络流支持
    }

    //FIXME this may make app crash, we should stop initialize;
    if (m_decoder->inputSource ()
            && !m_decoder->inputSource ()->isOpen ()
            && !m_decoder->inputSource ()->open (QIODevice::ReadOnly)) {
        qWarning()<<Q_FUNC_INFO<<QString("input error: %1").arg (m_decoder->inputSource ()->errorString ());
        if(m_handler->state() == PlayState::Stopped || m_handler->state() == PlayState::Buffering)
            m_handler->dispatch(PlayState::NormalError);
        return;
    }

    qDebug()<<Q_FUNC_INFO<<"Start initialize decoder";

    if (!m_decoder->initialize ()) {
        qWarning()<<Q_FUNC_INFO<<"invalid file format";
        //TODO: 使用pluginloader切换到另外一个支持当前格式的decoder
        return;
    }

//    m_audioParameters = m_decoder->audioParameters ();
     qDebug()<<Q_FUNC_INFO<<"m_decoder->audioParameters "<<m_decoder->audioParameters ()->parametersInfo ();

     if (m_audioParameters) {
         m_audioParameters->deleteLater ();
         m_audioParameters = nullptr;
     }
     m_audioParameters = new AudioParameters(m_decoder->audioParameters (), this);

     qDebug()<<Q_FUNC_INFO<<"current audioParameters "<<m_audioParameters->parametersInfo ();

    if (!m_decoder->durationInSeconds ()) {
        m_startPos = -1;
    } else if (startSec) {
        m_startPos = startSec * 1000;
    }
//    if (startPlay && !this->isRunning ()) {
//        qDebug()<<Q_FUNC_INFO<<"start run now";
//        start ();
//    }

    /////
//    if (startPlay) {
//        this->play (startSec);
//    } else {
//        this->setPosition (startSec);
//    }

}

void PlayThread::run()
{
    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>> PlayThread start loop <<<<<<<<<<<<<";

    m_mutex.lock ();
    m_next = false;
    qint64 len = 0;
//    int delay = 0;
    if (!m_decoder) {
        qWarning()<<Q_FUNC_INFO<<"No decoder found";
        m_mutex.unlock ();
        return;
    }
    addOffset (); //offset
    //TODO add replayGain
    m_mutex.unlock ();

    m_outputThread->start ();

    m_handler->dispatch (PlayState::Playing);
    m_handler->dispatch (m_decoder->durationInSeconds ());

    while (!m_done && !m_finish) {
        m_mutex.lock ();

//        qDebug()<<QString();
//        qDebug()<<QString();
//        qDebug()<<QString("======================== loop %1 ===============").arg (Q_FUNC_INFO);

        //seek
        if (m_seekTime >=0) {
            qDebug()<<Q_FUNC_INFO<<"m_seekTime "<<m_seekTime;
            m_decoder->setPosition (m_seekTime);
            m_seekTime = -1;
            m_outputThread->recycler ()->mutex ()->lock ();
            m_outputThread->recycler ()->clear ();
            m_outputThread->recycler ()->mutex ()->unlock ();
            m_output_at = 0;
        }
        //TODO if we'are using stream data, we should wait for data buffering, now we start output atm
        //TODO use replayGain or decoder, current use decoder
//        qDebug()<<Q_FUNC_INFO<<QString("m_output_size = [%1], m_output_at = [%2], m_output_size - m_output_at = [%3]")
//                  .arg (m_output_size).arg (m_output_at).arg (m_output_size - m_output_at);

        len = m_decoder->read ((char *)(m_output_buf + m_output_at), m_output_size - m_output_at);

//        qDebug()<<Q_FUNC_INFO<<"read bytes len is "<<len;

        if (len > 0) {
            m_bitrate = m_decoder->bitrate ();
            m_output_at += len;

//            qDebug()<<Q_FUNC_INFO<<QString("m_bitrate = [%1], now  m_output_at = [%2]")
//                      .arg (m_bitrate).arg (m_output_at);

            flush ();
        } else if (len == 0) {
            ////////////////////////////  decoder can play next track without initialization?
            /// ///////////////////////  get another decoder

            flush (true);
            if (m_outputThread) {
                m_outputThread->recycler ()->mutex ()->lock ();
                // end of stream
                while (!m_outputThread->recycler ()->empty () && !m_user_stop) {
                    m_outputThread->recycler ()->cond ()->wakeOne ();
                    m_mutex.unlock ();
                    m_outputThread->recycler ()->cond ()->wait (m_outputThread->recycler ()->mutex ());
                    m_mutex.lock ();
                }
                m_outputThread->recycler ()->mutex ()->unlock ();
            }
            m_done = true;
            m_finish = !m_user_stop;
        } else {
            m_finish = true;
        }
        m_mutex.unlock ();
    }
    m_decoder = nullptr;
    //FIXME clear decoders
//    m_pluginLoader->getCurrentPluginHost (Common::PluginDecoder)->unLoad ();
//    if (m_decoderHost) {
//        if (!m_decoderHost->unLoad ())
//            m_decoderHost->forceUnload ();
//        m_decoderHost->deleteLater ();
//    }
//    m_decoderHost = nullptr;

    m_mutex.lock ();
    m_next = false;
    if (m_finish)
        finish ();
    if (m_outputThread) {
        m_outputThread->mutex ()->lock ();
        m_outputThread->recycler ()->cond ()->wakeAll ();
        m_outputThread->mutex ()->unlock ();
    }
    m_mutex.unlock ();

    qDebug()<<Q_FUNC_INFO<<">>>>>>>>>>>>> PlayThread finish loop <<<<<<<<<<<<<";
}

void PlayThread::finish()
{
    if (m_outputThread) {
        m_outputThread->mutex ()->lock ();
        m_outputThread->finish ();
        m_outputThread->mutex ()->unlock ();
    }
    m_handler->sendFinished ();
}

void PlayThread::reset()
{
    m_done = false;
    m_finish = false;
    m_seekTime = -1;
    m_output_at = 0;
    m_user_stop = false;
    m_bitrate = 0;
    m_next = false;
//    m_use16BitOutputConvert = false;
}

void PlayThread::setAudioParameters(AudioParameters *other)
{
    if (m_audioParameters) {
        m_audioParameters->deleteLater ();
        m_audioParameters = nullptr;
    }
    m_audioParameters = new AudioParameters(other,this);
}

void PlayThread::flush(bool final)
{
    ulong min = final ? 0 : m_bks;

    qDebug()<<Q_FUNC_INFO<<" min "<<min;

    while ((!m_done && !m_finish) && m_output_at > min) {

        m_outputThread->recycler ()->mutex ()->lock ();

        while ((m_outputThread->recycler ()->full () || m_outputThread->recycler ()->blocked ())
               && (!m_done && !m_finish)) {
            if (m_seekTime > 0) {
                qDebug()<<Q_FUNC_INFO<<" m_seekTime "<<m_seekTime;
                m_output_at = 0;
                m_outputThread->recycler ()->mutex ()->unlock ();
                return;
            }

            m_mutex.unlock ();
            m_outputThread->recycler ()->cond ()->wait (m_outputThread->recycler ()->mutex ());
            m_mutex.unlock ();
            m_done = m_user_stop;
        }
        if (m_user_stop || m_finish) {
            m_done = true;
        } else {
            m_output_at -= produceSound ((char*)m_output_buf, m_output_at, m_bitrate);
        }
        if (!m_outputThread->recycler ()->empty ()) {
            m_outputThread->recycler ()->cond ()->wakeOne ();
        }
        m_outputThread->recycler ()->mutex ()->unlock ();
    }
}

void PlayThread::addOffset()
{
    qint64 pos = m_startPos;
    if (pos > 0) {
        qDebug()<<Q_FUNC_INFO<<" add pos "<<pos;
        m_seekTime = pos;
        m_outputThread->seek (pos);
    }
}

qint64 PlayThread::produceSound(char *data, qint64 size, quint32 brate)
{
//    qDebug()<<Q_FUNC_INFO<<QString("size(m_output_at) = [%1], m_bitrate = [%2]").arg (size).arg (brate);

    Buffer *b = m_outputThread->recycler ()->get ();
    uint sz = size < m_bks ? size : m_bks;
    memcpy (b->data, data, sz);
    b->nbytes = sz;
    b->rate = brate;

    if (m_use16BitOutputConvert && m_convertAudioParameters) {
        switch (m_convertAudioParameters->format ()) {
        case AudioParameters::PCM_S8: {
            unsigned char *out = new unsigned char[b->nbytes*2];
            s8_to_s16((qint8 *)b->data, (qint16 *) out, b->nbytes);
            delete [] b->data;
            b->data = out;
            b->nbytes <<= 1;
            break;
        }
        case AudioParameters::PCM_S24LE: {
            s24_to_s16((qint32 *)b->data, (qint16 *)b->data, b->nbytes >> 2);
            b->nbytes >>= 1;
            break;
        }
        case AudioParameters::PCM_S32LE: {
            s32_to_s16((qint32 *)b->data, (qint16 *)b->data, b->nbytes >> 2);
            b->nbytes >>= 1;
            break;
        }
        default:
            break;
        }
    }

    size -= sz;
    memmove (data, data + sz, size);
    m_outputThread->recycler ()->add ();
    return sz;

}

void PlayThread::createOutput()
{
//    OutputThread *m_ouputThread = new OutputThread(0, m_visual);
    if (!m_outputThread)
        m_outputThread = new OutputThread(0, m_visual);

    m_outputThread->reset ();
    m_outputThread->setMuted (m_muted);

    if (!m_audioParameters) {
        qCritical()<<Q_FUNC_INFO<<"No audioParameters found, will stop";
        return;
    }

    if (!m_outputThread->initialize (m_audioParameters->sampleRate (), m_audioParameters->channels (), m_audioParameters->format ())) {
        qWarning()<<Q_FUNC_INFO<<"initialize OutputThread fail";
        m_outputThread->deleteLater ();
        m_outputThread = nullptr;
        m_handler->dispatch (PlayState::FatalError);
        return;
    }
    if (!m_outputThread->audioParameters ()->equals (m_audioParameters)) {
        if (m_outputThread->audioParameters ()->format () != AudioParameters::PCM_S16LE) { //output supports 16 bit only
            qWarning()<<Q_FUNC_INFO<<"unsupported audio format";
            m_outputThread->deleteLater ();
            m_outputThread = nullptr;
            m_handler->dispatch (PlayState::FatalError);
            return;
        } else {
            m_use16BitOutputConvert = true;
            if (m_convertAudioParameters) {
                m_convertAudioParameters->deleteLater ();
                m_convertAudioParameters = nullptr;
            }
            m_convertAudioParameters = new AudioParameters(m_audioParameters,this);
            //change audio format info to PCM_S16LE
            if (m_audioParameters) {
                quint32 srate = m_audioParameters->sampleRate ();
                int chan = m_audioParameters->channels ();
                m_audioParameters->deleteLater ();
                m_audioParameters = nullptr;
                m_audioParameters = new AudioParameters(srate, chan, AudioParameters::PCM_S16LE, this);
            }
        }
    }
    if(m_output_buf)
        delete [] m_output_buf;
    m_bks = m_outputThread->recycler ()->blockSize ();
    m_output_size = m_bks * 4;
    m_output_buf = new unsigned char[m_output_size];
//    return m_outputThread;
}
OutputThread *PlayThread::output() const
{
    return m_outputThread;
}

void PlayThread::setOutput(OutputThread *output)
{
    m_outputThread = output;
}


} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
