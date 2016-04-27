#include "Decoder/IDecoder.h"

#include <QUrl>
#include <QDebug>
#include <qlogging.h>
#include <QIODevice>

#include "Buffer.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {

namespace Decoder {

IDecoder::IDecoder(QObject *parent)
    :QObject(parent)
//    ,mController(nullptr)
{
//    m_audioParameters = nullptr;
}

IDecoder::~IDecoder()
{
    qDebug()<<QString("========== %1 =======").arg (Q_FUNC_INFO);

//    if (m_audioParameters) {
//        m_audioParameters->deleteLater ();
//        m_audioParameters = nullptr;
//    }
}

void IDecoder::setInputSource(QIODevice *input)
{
    m_input = input;
}

QIODevice *IDecoder::inputSource()
{
    return m_input;
}

void IDecoder::setFileUri(const QString &uri)
{
    m_uri = uri;
}

QString IDecoder::fileUri() const
{
    return m_uri;
}

AudioParameters IDecoder::audioParameters() const
{
    return m_audioParameters;
}

void IDecoder::setAudioParameters(const AudioParameters &p)
{
    m_audioParameters = p;
}

//void IDecoder::configure(quint32 srate, int chan, AudioParameters::AudioFormat f)
//{
//    if (m_audioParameters) {
//        m_audioParameters->deleteLater ();
//        m_audioParameters = nullptr;
//    }
//    qDebug()<<Q_FUNC_INFO<<QString("sample = [%1], channel = [%2], format = [%3]")
//              .arg (srate).arg (chan).arg (f);

//    m_audioParameters = new AudioParameters(srate, chan, f);
//}

//void IDecoder::setController(PlayController *controller)
//{
//    mController = controller;
//}

//PlayController *IDecoder::getController()
//{
//    return mController;
//}

//bool IDecoder::open(const QUrl &url)
//{
//    AudioParameters *p = this->getAdudioParameters (url);
//    if (!p) {
//        qCritical()<<"Can't get the Audio Parameters!!!";
//        return false;
//    }
//    Buffer bin;
//    bin.audioParameters = p;
//    bin.buffer[0] = nullptr;
//    bin.buffer[1] = nullptr;

//    if (!mController) {
//        qCritical()<<"No PlayController pointer found!!";
//        return false;
//    }
//    mController->setOutBuffers (&bin, &bufferOut);

//    for (quint32 i=0; i<(mController->getBufferSize ())*(bufferOut->audioParameters->channels ()); ++i) {
//        bufferOut->buffer[0][i] = 0.0f;
//        bufferOut->buffer[1][i] = 0.0f;
//    }
//    qDebug()<<Q_FUNC_INFO<<QString("bufferOut[0] = [%1], bufferOut[1] = [%2]")
//              .arg (sizeof(bufferOut->buffer[0]))
//            .arg (sizeof(bufferOut->buffer[1]));
//    return true;

//}



} //Decoder
} //PhoenixPlayer
