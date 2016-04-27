#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

#include <QtGlobal>
#include <QSharedDataPointer>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {
class AudioParametersPriv;
class LIBPHOENIXPLAYER_EXPORT AudioParameters
{
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
    AudioParameters();
    AudioParameters(quint32 srate = 48000, quint32 chan = 2, AudioFormat f= AudioParameters::PCM_UNKNOWM);
    AudioParameters(const AudioParameters &other);
    AudioParameters &operator =(const AudioParameters &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    bool operator == (const AudioParameters &other);
    bool operator != (const AudioParameters &other) {
        return !operator == (other);
    }

    quint32 sampleRate() const;
    void setSampleRate(quint32 srate);

    quint32 channels() const;
    void setChannels(quint32 chan);

    AudioFormat format() const;
    void setFormat(AudioFormat f);

    int sampleSize() const;

    //Returns sample size in bytes of the given pcm data format.
    static int sampleSize(AudioFormat format);

    void printInfo();
    QString parametersInfo() const;
private:
    QSharedDataPointer<AudioParametersPriv> d;
};

} //PhoenixPlayer
#endif // AUDIOPARAMETERS_H
