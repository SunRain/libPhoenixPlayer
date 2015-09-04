#include "OutPut/IOutPut.h"


#include <QDebug>

#include "Buffer.h"
//#include "Player/PlayeController.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {

namespace OutPut {

IOutPut::IOutPut(QObject *parent)
    :QObject(parent)
{
//    mController == nullptr;
//    mWork = true;
//    mAudioParameters = new AudioParameters(0,0, AudioParameters::PCM_UNKNOWM);
    mAudioParameters = nullptr;
}

IOutPut::~IOutPut()
{
    //    mController = nullptr;
    if (mAudioParameters) {
        mAudioParameters->deleteLater ();
        mAudioParameters = nullptr;
    }
}

AudioParameters *IOutPut::audioParameters()
{
    return mAudioParameters;
}

void IOutPut::configure(quint32 srate, int chan, AudioParameters::AudioFormat f)
{
    if (mAudioParameters) {
        mAudioParameters->deleteLater ();
        mAudioParameters = nullptr;
    }
    mAudioParameters = new AudioParameters(srate, chan, f, this);
}

//void IOutPut::setWork(bool isWork)
//{
//    mWork = isWork;
//}

//bool IOutPut::isWork()
//{
//    return mWork;
//}

//void IOutPut::setController(PlayController *controller)
//{
//    mController = controller;
//}

//PlayController *IOutPut::getController()
//{
//    return mController;
//}

//void IOutPut::initialize(Buffer *in)
//{
//    qDebug()<<Q_FUNC_INFO;
//    if (!in || !mController) {
//        qCritical()<<"No input buffer found or no PlayController pointer!!";
//        return;
//    }
//    bufferIn = in;

//    if (!mWork)
//        mWork = !mWork;

//    while (isWork ()) {
//        if (!isWork ())
//            break;

//    }

//}




} //Decoder
} //PhoenixPlayer
