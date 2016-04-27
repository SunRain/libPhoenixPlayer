#include "AudioParameters.h"

#include <QDebug>
#include <QSharedData>

namespace PhoenixPlayer {

class AudioParametersPriv : public QSharedData
{
public:
    AudioParametersPriv() {
        srate = 48000;
        chan = 2;
        format = AudioParameters::PCM_UNKNOWM;
    }
    quint32 srate;
    quint32 chan;
    AudioParameters::AudioFormat format;
};
AudioParameters::AudioParameters()
    : d(new AudioParametersPriv)
{
}

AudioParameters::AudioParameters(quint32 srate, quint32 chan, AudioParameters::AudioFormat f)
    : d(new AudioParametersPriv)
{
    d.data ()->chan = chan;
    d.data ()->srate = srate;
    d.data ()->format = f;
}

AudioParameters::AudioParameters(const AudioParameters &other)
    : d(other.d)
{

}

bool AudioParameters::operator ==(const AudioParameters &other)
{
    return d.data ()->chan == other.d.data ()->chan
            && d.data ()->format == other.d.data ()->format
            && d.data ()->srate == other.d.data ()->srate;
}

quint32 AudioParameters::sampleRate() const
{
    return d.data ()->srate;
}

void AudioParameters::setSampleRate(quint32 srate)
{
    d.data ()->srate = srate;
}

quint32 AudioParameters::channels() const
{
    return d.data ()->chan;
}

void AudioParameters::setChannels(quint32 chan)
{
    d.data ()->chan = chan;
}

AudioParameters::AudioFormat AudioParameters::format() const
{
    return d.data ()->format;
}

void AudioParameters::setFormat(AudioParameters::AudioFormat f)
{
    d.data ()->format = f;
}


int AudioParameters::sampleSize() const
{
    return sampleSize(m_format);
}

int AudioParameters::sampleSize(AudioParameters::AudioFormat format)
{
    switch (format) {
    case PCM_S8:
        return 1;
    case PCM_S16LE:
    case PCM_UNKNOWM:
        return 2;
    case PCM_S24LE:
    case PCM_S32LE:
        return 4;
    default:
        return 2;
    }
    return 2;
}

void AudioParameters::printInfo()
{
    qDebug()<<Q_FUNC_INFO<<parametersInfo ();
}

QString AudioParameters::parametersInfo() const
{
    QString str;
    switch (d.data ()->format) {
    case PCM_S8:
        str = "PCM_S8";
        break;
    case PCM_S16LE:
    case PCM_UNKNOWM:
        str = "PCM_S16LE or PCM_UNKNOWM";
        break;
    case PCM_S24LE:
    case PCM_S32LE:
        str = "PCM_S24LE or PCM_S32LE";
        break;
    default:
        str = "default PCM_S16LE or PCM_UNKNOWM";
        break;
    }
    return QString("SampleRate = [%1], Channels = [%2], AudioFormat = [%3]")
            .arg(d.data ()->srate).arg(d.data ()->chan).arg (str);
}

} //PhoenixPlayer
