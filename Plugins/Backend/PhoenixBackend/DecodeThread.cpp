#include "DecodeThread.h"

#include <QDebug>
#include <QCoreApplication>

#include "AudioParameters.h"
#include "Buffer.h"
#include "PluginLoader.h"
#include "PluginHost.h"
#include "PPCommon.h"
#include "PPSettings.h"
#include "LibPhoenixPlayerMain.h"
#include "MediaResource.h"

#include "Decoder/IDecoder.h"
#include "Decoder/DecoderHost.h"

#include "BufferQueue.h"
#include "StateHandler.h"
#include "AudioConverter.h"
#include "ChannelConverter.h"
#include "AudioEffect.h"

using namespace PhoenixPlayer::Decoder;
using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;

const static int TRANSPORT_TIMEOUT = 5000; //ms

DecodeThread::DecodeThread(StateHandler *handle, BufferQueue *queue, QList<AudioEffect *> *list, QObject *parent)
    : QThread (parent),
    m_handler(handle),
    m_bufferQueue(queue),
//    m_converter(converter),
    m_effectList(list)
{
    m_pluginLoader = phoenixPlayerLib->pluginLoader();
    m_settings = phoenixPlayerLib->settings();

    m_decoderLibs = m_settings->decoderLibraries();
    if (m_decoderLibs.isEmpty()) {
        m_decoderLibs = m_pluginLoader->pluginLibraries(PPCommon::PluginDecoder);
    }
    qDebug()<<Q_FUNC_INFO<<" m_decoderLibs "<<m_decoderLibs
           <<" all decoderLibs "<<m_pluginLoader->pluginLibraries(PPCommon::PluginDecoder);
}

DecodeThread::~DecodeThread()
{
    stopDecoding();
    if (m_output_buf) {
        delete [] m_output_buf;
    }
    m_output_buf = Q_NULLPTR;
    if (m_audioConverter) {
        delete m_audioConverter;
    }
    m_audioConverter = Q_NULLPTR;
}

bool DecodeThread::changeMedia(PhoenixPlayer::MediaResource *res, quint64 startSec)
{
    this->stopDecoding();
    this->reset();

    if (m_output_buf) {
        delete [] m_output_buf;
        m_output_buf = Q_NULLPTR;
    }
    if (!res) {
        return false;
    }
    if (m_resource) {
        m_resource->deleteLater();
        m_resource = Q_NULLPTR;
    }
    m_resource = res;

    if (m_decoder) {
        //TODO 判断当前decoder是否支持解析当前的media
    } else {
        //TODO 使用PluginLoader装载不同的Decoder，需要添加Decoder是否支持当前媒体的接口
        foreach (const QString &s, m_decoderLibs) {
            m_decoderHost = new DecoderHost(s);
            if (m_decoderHost->isValid()) { //TODO 需要判断当前decoder/host是否支持当前媒体的解码
                m_decoder = m_decoderHost->instance<IDecoder>();
                if (!m_decoder) {
                    if (!m_decoderHost->unLoad()) {
                        m_decoderHost->forceUnload();
                    }
                    m_decoder = nullptr;
                    m_decoderHost->deleteLater();
                    m_decoderHost = nullptr;
                    continue;
                }
                break;
            }
        }
    }

    if (!m_decoder) {
        qCritical()<<Q_FUNC_INFO<<"No decoder found!!";
        return false;
    }

    //TODO post state to backend
    if (!res->initialize()) {
        qWarning()<<Q_FUNC_INFO<<"initialize error !";
        return false;
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
    m_audioParameters = m_decoder->audioParameters();

    qDebug()<<Q_FUNC_INFO<<"current audioParameters "<<m_audioParameters.parametersInfo ();

    if (!m_decoder->durationInSeconds()) {
        m_seekTimeMS = -1;
    } else if (startSec) {
        m_seekTimeMS = startSec * 1000;
    }

    m_bitrate = m_decoder->bitrate();
    m_blockSize = Buffer::BUFFER_PERIOD * m_audioParameters.frameSize();
    m_outputSize = m_blockSize * 4;
    m_sample_size = m_audioParameters.sampleSize();
    m_output_buf = new unsigned char[m_outputSize];

    if (!m_audioConverter) {
        m_audioConverter = new AudioConverter();
    }
    m_audioConverter->setFormat(m_audioParameters.format());
    // force set format
    m_audioParameters = AudioParameters(m_audioParameters.sampleRate(),
                                        m_audioParameters.channels(),
                                        AudioParameters::PCM_FLOAT);

    if (m_channelConverter) {
        delete m_channelConverter;
        m_channelConverter = Q_NULLPTR;
    }
    if (m_audioParameters.channels() != m_audioParameters.remapedChannels()) {
        qDebug()<<Q_FUNC_INFO<<"Use channel converter!!";
        m_channelConverter = new ChannelConverter(m_audioParameters.remapedChannels());
        m_channelConverter->initialization(m_audioParameters.sampleRate(),
                                             m_audioParameters.channels());
        m_audioParameters = m_channelConverter->generateAudioParameters();
    } else {
        qDebug()<<Q_FUNC_INFO<<"Not use channel converter!!";
    }
    if (this->isRunning()) {
        qDebug()<<Q_FUNC_INFO<<"thread is still running, terminate!!!!";
        this->terminate();
        this->wait();
    }
    return true;
}

void DecodeThread::stopDecoding()
{
    this->m_mutex.lock();
    m_user_stop = true;
    this->m_mutex.unlock();

    m_bufferQueue->mutex()->lock();
    m_bufferQueue->clear();
    m_bufferQueue->mutex()->unlock();

    m_bufferQueue->waitOut()->wakeAll();
    //FIXME should wake waitIn?

    int loopOut = 3;
    do {
        qDebug()<<Q_FUNC_INFO<<"======= thread is runnging, loopout "<<loopOut;
        loopOut--;
        qApp->processEvents();
    } while(this->isRunning() && loopOut > 0);

    if (this->isRunning()) {
        qDebug()<<Q_FUNC_INFO<<"======== thread still running";
    }

    m_bufferQueue->mutex()->lock();
    m_bufferQueue->clear();
    m_bufferQueue->mutex()->unlock();

    m_bufferQueue->waitOut()->wakeAll();
    //FIXME should wake waitIn?
}

void DecodeThread::seekMS(qint64 millisecond)
{
    qDebug()<<Q_FUNC_INFO<<"=== seek to "<<millisecond;

    QMutexLocker ml_1(m_bufferQueue->mutex());
    QMutexLocker ml_2(&m_mutex);

    m_bufferQueue->clear();
    m_seekTimeMS = millisecond;
}

PhoenixPlayer::AudioParameters DecodeThread::audioParameters() const
{
    return m_audioParameters;
}

bool DecodeThread::event(QEvent *event)
{
    return QThread::event(event);
}


void DecodeThread::run()
{
    if (!m_decoder) {
        qDebug()<<Q_FUNC_INFO<<"No decoder found!!";
        return;
    }
    m_handler->dispatch(PlayState::Buffering);
    m_handler->dispatchTrackTimeMS(m_decoder->durationInSeconds() * 1000);
    m_handler->dispatch(PlayState::Playing);

    int pendingDely = 0;
    bool decodeFinish = false; // flag to indicate decode finished
    bool decodeTerminate = false; // flag to indicat decode stopped by errors
    while (true) {
        if (decodeFinish || decodeTerminate) {
            break;
        }

        //FIXME QMutexLocker will use more sytem resource ? use mutext.lock && unlock?
        QMutexLocker locker(&m_mutex);

        if (m_user_stop) break;

        // check resource initialization
        {
            // seek to ms and reset to -1 for pending wait loop
            if (m_seekTimeMS >= 0) {
                m_decoder->setPositionMS(m_seekTimeMS);
                m_handler->dispatchSeekTime(m_seekTimeMS);
                m_seekTimeMS = -1;
                m_bufferQueue->mutex()->lock();
                m_bufferQueue->clear();
                m_bufferQueue->mutex()->unlock();
            }
            // check for pending if network resource
            if (m_resource->pending()) {
                msleep(5);
                pendingDely += 5;
                if (pendingDely >= TRANSPORT_TIMEOUT) {
                    m_handler->dispatch(PlayState::NormalError);
                    //TODO break flag?
                    decodeTerminate = true;
                    break;
                }
                continue;
            }
            pendingDely = 0;
        }

        if (m_user_stop || decodeFinish || decodeTerminate) break;

        // read from decoder and add to buffer queue
        {
#if 0
            quint64 pos = 0;
            //loop to read data to tmp buffer
            do {
                qint64 len = m_decoder->runDecode(m_output_buf + pos, m_outputSize - pos);
                if (len > 0) {
                    pos += len;
                    if (pos == m_outputSize) {
//                        decodeFinish = true;
                        break;
                    }
                } else if (len == 0) {
//                    decodeFinish = true;
                    break;
                } else {
                    decodeTerminate = true;
                    break;
                }
                locker.unlock();
                if (m_user_stop) {
                    decodeTerminate = true;
                    locker.relock();
                    break;
                }
                locker.relock();
            } while (true);
            if (m_user_stop || decodeTerminate) break;

            m_bufferQueue->mutex()->lock();
            if (m_bufferQueue->size() == m_bufferQueue->maxAvailableCnt()) {
                qDebug()<<Q_FUNC_INFO<<"Size in max size, wait to out";
                m_bufferQueue->waitOut()->wait(m_bufferQueue->mutex());
            }
            m_bufferQueue->mutex()->unlock();

            qDebug()<<Q_FUNC_INFO<<" not full, continue to append to queue";

            size_t samples = m_outputSize / m_sample_size;
            Buffer *b = new Buffer(m_audioParameters.channels().count() * Buffer::BUFFER_PERIOD);
            b->samples = samples;
            b->rate = m_bitrate;
            b->lastBuffer = (decodeFinish || decodeTerminate);

            qDebug()<<Q_FUNC_INFO<<" samples "<<samples<<" buffer size "<<b->size;

            m_audioConverter->toFloat(m_output_buf, b->data, samples);

            foreach (auto e, *m_effectList) {
                e->apply(b);
            }
            if (m_channelConverter) {
                m_channelConverter->apply(b);
            }

            m_bufferQueue->mutex()->lock();
            m_bufferQueue->enqueue(b);
            m_bufferQueue->waitIn()->wakeAll();
            m_bufferQueue->mutex()->unlock();
#endif
            qint64 len = m_decoder->runDecode(m_output_buf + m_output_at, m_outputSize - m_output_at);
            if (len > 0) {
                m_output_at += len;

                bool seek = false;
                while (m_output_at > m_blockSize) {
                    locker.unlock();
                    if (m_user_stop) {
                        decodeTerminate = true;
                        locker.relock();
                        break;
                    }
                    if (m_seekTimeMS > 0) {
                        m_output_at = 0;
                        seek = true;
                        break;
                    }
                    locker.relock();

                    m_bufferQueue->mutex()->lock();

//                    qDebug()<<Q_FUNC_INFO<<" queue size "<<m_bufferQueue->size()
//                             <<" maxAvailableCnt "<<m_bufferQueue->maxAvailableCnt();
                    if (m_bufferQueue->size() == m_bufferQueue->maxAvailableCnt()) {
                        qDebug()<<Q_FUNC_INFO<<"Size in max size, wait to out";
                        m_bufferQueue->waitOut()->wait(m_bufferQueue->mutex());
                    }
                    m_bufferQueue->mutex()->unlock();

//                    qDebug()<<Q_FUNC_INFO<<" not full, continue to append to queue";

                    size_t sz = m_output_at < m_blockSize ? m_output_at : m_blockSize;
                    size_t samples = sz / m_sample_size;

                    Buffer *b = new Buffer(m_audioParameters.channels().count() * Buffer::BUFFER_PERIOD);
                    b->samples = samples;
                    b->rate = m_bitrate;
                    b->lastBuffer = (decodeFinish || decodeTerminate);

//                    qDebug()<<Q_FUNC_INFO<<" samples "<<samples<<" buffer size "<<b->size;

                    m_audioConverter->toFloat(m_output_buf, b->data, samples);

                    foreach (auto e, *m_effectList) {
                        e->apply(b);
                    }
                    if (m_channelConverter) {
                        qDebug()<<Q_FUNC_INFO<<"use channelConverter";
                        m_channelConverter->apply(b);
                    }

                    m_bufferQueue->mutex()->lock();
                    m_bufferQueue->enqueue(b);
                    m_bufferQueue->waitIn()->wakeAll();
                    m_bufferQueue->mutex()->unlock();

                    m_output_at -= sz;
                }
                if (seek) {
                    continue;
                }

            } else if (len == 0) { //decoder finish

            } else { //decoder error

            }

        }
    }
    if (decodeFinish || decodeTerminate) {
        m_bufferQueue->mutex()->lock();
        if (!m_bufferQueue->isEmpty()) {
            m_bufferQueue->last()->lastBuffer = true;
        }
        m_bufferQueue->mutex()->unlock();
    }
}

void DecodeThread::reset()
{
    m_seekTimeMS = -1; //seek time in milliseconds;
    m_user_stop = false;
    m_blockSize = 0;
    m_sample_size = 0;
    m_bitrate = 0;
    m_output_at = 0;
}
