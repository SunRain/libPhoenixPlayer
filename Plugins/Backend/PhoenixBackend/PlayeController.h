#ifndef PLAYECONTROLLER_H
#define PLAYECONTROLLER_H

#include <QObject>
#include <QMutex>
#include <QThread>

#include "Buffer.h"

class QThread;
namespace PhoenixPlayer {

namespace Decoder {
class IDecoder;
}

namespace OutPut {
class IOutPut;
}

class PluginLoader;
class PluginHost;
//class Buffer;
class PlayController : public QObject
{
    Q_OBJECT
    friend class PlayThread;
public:
    explicit PlayController(QObject *parent = 0);
    virtual ~PlayController();

    // mutex[0..1] for buffer, mutex[2..3] for buffer2
    QMutex mutex[2];

    // internal, play_worker runs only if work==true, if not it MUST return
    bool work;
    // internal, paused state
    bool paused;

    void setOutBuffers(Buffer *outProp, Buffer **out);

    qint32 getBufferSize() const;

public slots:
    void open(const QUrl &url);
    void play();
    void stop();
    void pause();

private:
    class PlayThread : public QThread
    {
        Q_OBJECT
    public:
        explicit PlayThread(QObject *parent = 0) : QThread(parent) {}
        virtual ~PlayThread() {}
        void setPlayControler(PlayController *controller) {
            mController = controller;
        }
        // QThread interface
    protected:
        void run();
    private:
        PlayController *mController;
    };
    PlayThread *mInternalThread;
private:
    bool mActive;
    // mBuffeOut: buffer out from decoder plugin
    Buffer mBuffeOut;
    // mBufferIn: buffer in for output plugin
    Buffer mBufferIn;

    PluginHost *mDecoderHost;
    PluginHost *mOutPutHost;
    Decoder::IDecoder *mDecoder;
    OutPut::IOutPut *mOutPut;

    PluginLoader *mPluginLoader;

    int mBufferCursor;
    int mBufferSamples[2];

    QThread *mDecoderThread;
    QThread *mOutPutThread;
};


} //PhoenixPlayer
#endif // PLAYECONTROLLER_H
