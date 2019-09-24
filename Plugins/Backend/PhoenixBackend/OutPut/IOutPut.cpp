#include "IOutPut.h"

#include <QDebug>

namespace PhoenixPlayer {

    namespace PlayBackend {

        namespace PhoenixBackend {

            namespace OutPut {

IOutPut::IOutPut(QObject *parent)
    : QObject(parent)
{

}

IOutPut::~IOutPut()
{
}

bool IOutPut::initialize(quint32 sampleRate, const ChannelMap &map, AudioParameters::AudioFormat f)
{
    m_sampleRate = sampleRate;
//    m_channels = map;
    m_chan_map = map;
    m_format = f;
    m_sampleSize = AudioParameters::sampleSize(f);
    return true;
}

AudioParameters IOutPut::audioParameters() const
{
    return AudioParameters(m_sampleRate, /*m_channels*/m_chan_map, m_format);
}

quint32 IOutPut::sampleRate() const
{
    return m_sampleRate;
}

const ChannelMap IOutPut::channelMap() const
{
    return m_chan_map;
}

int IOutPut::channels() const
{
    return m_chan_map.count();
}

//QList<AudioParameters::ChannelPosition> IOutPut::channels() const
//{
////    return m_channels;
//return m_chan_map.count();
//}

AudioParameters::AudioFormat IOutPut::format() const
{
    return m_format;
}

int IOutPut::sampleSize() const
{
    return m_sampleSize;
}


} //OutPut
} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
