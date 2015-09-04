#include "AudioParameters.h"

#include <QDebug>

namespace PhoenixPlayer {

AudioParameters::AudioParameters(QObject *parent)
    :BaseObject(parent)
{
    m_srate = 0;
    m_chan = 0;
    m_format = AudioFormat::PCM_S16LE;
}

AudioParameters::AudioParameters(quint32 srate, int chan, AudioFormat f, QObject *parent)
    :BaseObject(parent)
{
    m_srate = srate;
    m_chan = chan;
    m_format = f;
}

AudioParameters::AudioParameters(AudioParameters *other, QObject *parent)
    :BaseObject(parent)
{
    m_srate = other->sampleRate ();
    m_chan = other->channels ();
    m_format = other->format ();
}

quint32 AudioParameters::sampleRate() const
{
    return m_srate;
}

//void AudioParameters::setSampleRate(quint32 srate)
//{
//    m_srate = srate;
//}

int AudioParameters::channels() const
{
    return m_chan;
}

//void AudioParameters::setChannels(int chan)
//{
//    m_chan = chan;
//}

AudioParameters::AudioFormat AudioParameters::format() const
{
    return m_format;
}

//void AudioParameters::setFormat(AudioParameters::AudioFormat f)
//{
//    m_format = f;
//}

bool AudioParameters::equals(AudioParameters *other)
{
    return m_srate == other->sampleRate ()
            && m_chan == other->channels ()
            && m_format == other->format ();
}

int AudioParameters::sampleSize() const
{
    return sampleSize(m_format);
}

//void AudioParameters::operator =(const AudioParameters &p)
//{
//    mSrate = p.sampleRate ();
//    mChan = p.channels ();
//    mFormat = p.format ();
//}

//bool AudioParameters::operator ==(const AudioParameters &p)
//{
//    return mSrate == p.sampleRate () && mChan == p.channels () && mFormat == p.format ();
//}

//bool AudioParameters::operator !=(const AudioParameters &p)
//{
//    return !(operator == (p));
//}

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

QString AudioParameters::parametersInfo()
{
    return QString("SampleRate = [%1], Channels = [%2], AudioFormat = [%3]")
            .arg(m_srate).arg(m_chan).arg (enumToStr ("AudioFormat", (int)m_format));
}

} //PhoenixPlayer
