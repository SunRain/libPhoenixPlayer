#include "AudioEffect.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

AudioEffect::AudioEffect()
{

}

AudioEffect::~AudioEffect()
{

}

void AudioEffect::initialization(quint32 sampleRate, const ChannelMap &map)
{
    m_sampleRate = sampleRate;
    m_chan_map = map;
}

const AudioParameters AudioEffect::generateAudioParameters() const
{
    return AudioParameters(m_sampleRate, m_chan_map, AudioParameters::PCM_FLOAT);
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
