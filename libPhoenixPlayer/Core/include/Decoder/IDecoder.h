#ifndef IDECODER_H
#define IDECODER_H

#include <QObject>
#include <QtGlobal>
#include "AudioParameters.h"

class QUrl;
class QIODevice;
namespace PhoenixPlayer {

class Buffer;
class PlayController;
namespace Decoder {

class IDecoder : public QObject
{
    Q_OBJECT
public:
    IDecoder(QObject *parent = 0);
    virtual ~IDecoder();
    void setInputSource(QIODevice *input = 0);
    QIODevice *inputSource();

    void setFileUri(const QString &uri);
    QString fileUri() const;

    virtual bool initialize() = 0;
    virtual quint64 getLength() = 0;
    virtual void setPosition(qreal pos = 0) = 0;
//    virtual qreal getPosition() = 0;
    /*!
     * Returns current bitrate (in kbps).
     * Subclass should reimplement this function.
     */
    virtual int bitrate() = 0;

    /*
     * Reads up to maxSize bytes of decoded audio to data
     * Returns the number of bytes read, or -1 if an error occurred.
     * Subclass should reimplement this function.
     */
    virtual qint64 read(char *data, qint64 maxSize) = 0;
//    virtual void setController(PlayController *controller = 0);
//    PlayController *getController();
//    virtual bool open(const QUrl &url);

//    virtual AudioParameters *getAdudioParameters(const QUrl &url) = 0;

//    virtual bool reader() = 0;

    AudioParameters *audioParameters() const;

protected:
//    Buffer *bufferOut;
    void configure(quint32 srate = 44100, int chan = 2, AudioParameters::AudioFormat f = AudioParameters::PCM_S16LE);
private:
//    PlayController *mController;
    QIODevice *m_input;
    AudioParameters *m_audioParameters;
    QString m_uri;
};
} //Decoder
} //PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::Decoder::IDecoder, "PhoenixPlayer.Decoder.BaseDecoderInterface/1.0")
#endif // IDECODER_H
