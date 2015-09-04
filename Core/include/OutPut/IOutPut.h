#ifndef IOUTPUT_H
#define IOUTPUT_H

#include <QObject>
#include "AudioParameters.h"

namespace PhoenixPlayer {

//class PlayController;
class Buffer;
class AudioParameters;
namespace OutPut {

class IOutPut : public QObject
{
    Q_OBJECT
public:
    IOutPut(QObject *parent = 0);
    virtual ~IOutPut();
//    void setWork(bool isWork = false);
//    bool isWork();
//    virtual void setController(PlayController *controller = 0);
//    PlayController *getController();

//    /*
//     * 根据不同插件返回 BUFFER_PERIOD倍数
//     */
//    virtual qint32 getBufferSize() = 0;

//    virtual void initialize(Buffer *in = 0) = 0;
    virtual bool initialize(quint32 srate, int chan, AudioParameters::AudioFormat f) = 0;
    /*
     * Returns output interface latency in milliseconds.
     */
    virtual qint64 latency() = 0;
//    virtual int writeAudio(Buffer *buffer = 0) = 0;
//    virtual void rewind() = 0;
//    virtual void resume() = 0;
//    virtual void pause() = 0;
    /*
     * Writes up to maxSize bytes from  data to the output interface device.
     * Returns the number of bytes written, or -1 if an error occurred.
     * Subclass should reimplement this function.
     */
    virtual qint64 writeAudio(unsigned char *data, qint64 maxSize) = 0;
    /*
     * Writes all remaining plugin's internal data to audio output device.
     * Subclass should reimplement this function.
     */
    virtual void drain() = 0;
    /*
     * Drops all plugin's internal data, resets audio device
     * Subclass should reimplement this function.
     */
    virtual void reset() = 0;
    /*
     * Stops processing audio data, preserving buffered audio data.
     */
    virtual void suspend() {}
    /*
     * Resumes processing audio data.
     */
    virtual void resume() {}

     AudioParameters *audioParameters();

protected:
//    Buffer *bufferIn;
     /*
      * Use this function inside initialize() reimplementation to tell about accepted audio parameters.
      * @param freq Sample rate.
      * @param chan Number of channels.
      * @param format Audio format.
      */
     void configure(quint32 srate, int chan, AudioParameters::AudioFormat f);
private:
//    PlayController *mController;
//    bool mWork;
     AudioParameters *mAudioParameters;
//     quint32 mSrate;
//     int mChan;
};
} //Decoder
} //PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::OutPut::IOutPut, "PhoenixPlayer.OutPut.BaseOutPutInterface/1.0")
#endif // IOUTPUT_H
