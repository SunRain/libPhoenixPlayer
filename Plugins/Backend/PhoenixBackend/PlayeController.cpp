#include "Player/PlayeController.h"

#include <QDebug>
#include <QThread>

#include "Decoder/IDecoder.h"
#include "OutPut/IOutPut.h"
#include "AudioParameters.h"
#include "Buffer.h"
#include "PluginLoader.h"
#include "SingletonPointer.h"
#include "PluginHost.h"

namespace PhoenixPlayer {

PlayController::PlayController(QObject *parent)
    :QThread(parent)
{
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mPluginLoader = PluginLoader::instance();
#else
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
    mActive = false;
    work = false;
    paused = true;

//    mDecoderThread = new QThread(this);
    mOutPutThread = new QThread(this);
    mInternalThread = new PlayThread(this);
    mInternalThread->setPlayControler (this);

    mOutPutHost = nullptr;
    mDecoderHost = nullptr;
    mOutPut = nullptr;
    mDecoder = nullptr;

    mBufferCursor = 0;

    mBuffeOut.audioParameters = nullptr;
    mBuffeOut.buffer[0] = nullptr;
    mBuffeOut.buffer[1] = nullptr;

    mutex[1].tryLock ();
}

PlayController::~PlayController()
{

}

void PlayController::setOutBuffers(Buffer *outProp, Buffer **out)
{
    *out = &mBuffeOut;
    mBufferCursor = 0;
    mBufferSamples[0] = 0;
    mBufferSamples[1] = 0;

    if ((!mBuffeOut.audioParameters) || (mBuffeOut.audioParameters != outProp->audioParameters)) {
        qDebug()<<"Audio Parameters not same";
        if (mOutPutThread->isRunning ()) {
            mOutPutThread->quit ();
            if (mOutPutThread->wait ())
                qDebug()<<"stop mOutPutThread OK";
            else
                qDebug()<<"stop mOutPutThread fail";
            mOutPutHost->unLoad ();
            mOutPut = nullptr;
        }
        if (mBuffeOut.buffer[0]) {
            free (mBuffeOut.buffer[0]);
            mBuffeOut.buffer[0] = nullptr;
            free (mBuffeOut.buffer[1]);
            mBuffeOut.buffer[1] = nullptr;
        }
        mOutPut = mPluginLoader->getCurrentOutPut ();
        mOutPut->setController (this);
        Q_CHECK_PTR(mOutPut);

        outProp->buffer[0]
                = (float*)aligned_alloc(sizeof(void *)*2,
                                                   (sizeof(float)
                                                    *(getBufferSize ())
                                                    *(outProp->audioParameters->channels ())
                                                    )
                                        );
        outProp->buffer[1]
                = (float*)aligned_alloc(sizeof(void *)*2,
                                                   (sizeof(float)
                                                    *(getBufferSize ())
                                                    *(outProp->audioParameters->channels ())
                                                    )
                                        );
        outProp->cursor = &mBufferCursor;
        Q_CHECK_PTR(outProp->buffer[0]);
        Q_CHECK_PTR(outProp->buffer[1]);
        mBuffeOut.audioParameters = outProp->audioParameters;
        mBuffeOut.buffer[0] = outProp->buffer[0];
        mBuffeOut.buffer[1] = outProp->buffer[1];
        mBuffeOut.cursor = &mBufferCursor;

        mBufferCursor = 0;

        qDebug()<<Q_FUNC_INFO<<mBuffeOut.audioParameters->printInfo ();

        //TODO: add eq functions

        memcpy (&mBufferIn, &mBuffeOut, sizeof(Buffer));


        if (!mOutPutThread)
            mOutPutThread = new QThread(this);
        mOutPut->moveToThread (mOutPutThread);
        mOutPutThread->run ();

        mOutPut->writeAudio (&mBufferIn);
    }

}

qint32 PlayController::getBufferSize() const
{
    return mOutPut->getBufferSize ();
}

void PlayController::open(const QUrl &url)
{
    stop ();

//    if (!mDecoderThread) {
//        mDecoderThread = new QThread(this);
//    }

//    if (mDecoderThread->isRunning ()) {
//        mDecoderThread->quit ();
//        if (!mDecoderThread->wait ())
//            qDebug()<<Q_FUNC_INFO<<"quit mDecoderThread fail";
//        else
//            qDebug()<<Q_FUNC_INFO<<"quit mDecoderThread ok";
//    }

    //TODO use different decoder
    if (!mDecoder)
        mDecoder = mPluginLoader->getCurrentDecoder ();
    if (!mDecoderHost)
        mDecoderHost = mPluginLoader->getCurrentPluginHost (Common::PluginDecoder);

    mDecoder->setController (this);

    if (!mDecoder) {
        qCritical()<<"No decoder found!!";
        return;
    }

//    mDecoder->moveToThread (mDecoderThread);

//    mDecoderThread->start ();

    mDecoder->open (url);

    work = true;
    mActive = true;
    paused = false;

    mInternalThread->start ();
    mOutPut->resume ();

}

void PlayController::play()
{
    if (mDecoder && paused) {
        paused = false;
        mOutPut->resume ();
    }
}

void PlayController::stop()
{
    if (mOutPut) {
        work = false;
        mOutPut->resume ();
        mInternalThread->quit ();
        if (mInternalThread->wait ()) {
            qDebug()<<"stop mInternalThread ok";
        } else {
            qDebug()<<"stop mInternalThread fail";
        }
        paused = true;
    }
}

void PlayController::pause()
{
    if (mDecoder && !paused) {
        mOutPut->pause ();
        paused = true;
    }
}

void PlayController::PlayThread::run()
{
    qDebug()<<Q_FUNC_INFO<<"Start internal thread";
    while (true) {
        this->mController->mDecoder->reader ();
//        if (this->mController->mActive) {
//            this->mController->mActive = false;
//        }
//        qDebug()<<"Wait for output write finish";
//        mController->mutex[0].lock ();
//        mController->mutex[0].unlock ();
//        qDebug()<<"done";
    }

}



} //PhoenixPlayer




















