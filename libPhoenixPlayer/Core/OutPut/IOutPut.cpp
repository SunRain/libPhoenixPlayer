#include "OutPut/IOutPut.h"

#include <QDebug>

namespace PhoenixPlayer {

namespace OutPut {

IOutPut::IOutPut(QObject *parent)
    : QObject(parent)
{

}

IOutPut::~IOutPut()
{
}

bool IOutPut::initialize(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &channels, AudioParameters::AudioFormat f)
{
    m_sampleRate = sampleRate;
    m_channels = channels;
    m_format = f;
    m_sampleSize = AudioParameters::sampleSize(f);
    return true;
}

AudioParameters IOutPut::audioParameters() const
{
    return AudioParameters(m_sampleRate, m_channels, m_format);
}

quint32 IOutPut::sampleRate() const
{
    return m_sampleRate;
}

QList<AudioParameters::ChannelPosition> IOutPut::channels() const
{
    return m_channels;
}

AudioParameters::AudioFormat IOutPut::format() const
{
    return m_format;
}

int IOutPut::sampleSize() const
{
    return m_sampleSize;
}


} //Decoder
} //PhoenixPlayer
