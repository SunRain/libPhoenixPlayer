#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

#include <QtGlobal>
#include <QObject>
#include <QDebug>

#include "BaseObject.h"

namespace PhoenixPlayer {
class AudioParameters : public BaseObject
{
    Q_OBJECT
    Q_ENUMS(AudioFormat)
public:
    //Audio formats
    enum AudioFormat {
        PCM_S8 = 0x200,     //Signed 8 bit
        PCM_S16LE,          //Signed 16 bit Little Endian
        PCM_S24LE,          //Signed 24 bit Little Endian using low three bytes in 32-bit word
        PCM_S32LE,          //Signed 32 bit Little Endian
        PCM_UNKNOWM         //Unknown format
    };
public:
    explicit AudioParameters(QObject *parent = 0);
    explicit AudioParameters(quint32 srate, int chan, AudioFormat f, QObject *parent = 0);
    explicit AudioParameters(AudioParameters *other, QObject *parent = 0);

    quint32 sampleRate() const;
//    void setSampleRate(quint32 srate);

    int channels() const;
//    void setChannels(int chan);

    AudioFormat format() const;
//    void setFormat(AudioFormat f);

    bool equals(AudioParameters *other);

//    void operator = (const AudioParameters &p);
//    bool operator ==(const AudioParameters &p);
//    bool operator !=(const AudioParameters &p);

    int sampleSize() const;

    //Returns sample size in bytes of the given pcm data format.
    static int sampleSize(AudioFormat format);

    void printInfo();
    QString parametersInfo();
private:
    quint32 m_srate;
    int m_chan;
    AudioFormat m_format;
};

} //PhoenixPlayer
#endif // AUDIOPARAMETERS_H
