#include "AudioEffect.h"

using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;

AudioEffect::AudioEffect()
{

}

AudioEffect::~AudioEffect()
{

}

void AudioEffect::initialization(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &list)
{
    m_sampleRate = sampleRate;
    m_list = list;
}

const PhoenixPlayer::AudioParameters AudioEffect::generateAudioParameters() const
{
    return AudioParameters(m_sampleRate, m_list, AudioParameters::PCM_FLOAT);
}
