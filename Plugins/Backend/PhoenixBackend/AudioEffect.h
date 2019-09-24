#ifndef AUDIOEFFECT_H
#define AUDIOEFFECT_H

#include <QtGlobal>

#include "AudioParameters.h"

namespace PhoenixPlayer {
    namespace PlayBackend {
        namespace PhoenixBackend {
            class Buffer;

class AudioEffect
{
public:
    AudioEffect();
    virtual ~AudioEffect();

    virtual void apply(Buffer *buffer) = 0;

//    virtual void initialization(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &list);
    virtual void initialization(quint32 sampleRate, const ChannelMap &map);

    ///
    /// \brief generateAudioParameters
    /// \return audio parameters with AudioFormat::PCM_FLOAT
    ///
    const AudioParameters generateAudioParameters() const;

private:
    quint32                                 m_sampleRate = 0;
//    QList<AudioParameters::ChannelPosition> m_list;
    ChannelMap m_chan_map;

};


} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // AUDIOEFFECT_H
