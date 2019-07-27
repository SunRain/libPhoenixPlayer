#ifndef AUDIOEFFECT_H
#define AUDIOEFFECT_H

#include <QtGlobal>

#include "AudioParameters.h"

namespace PhoenixPlayer {
    class Buffer;

    namespace PlayBackend {

        namespace PhoenixBackend {

class AudioEffect
{
public:
    AudioEffect();
    virtual ~AudioEffect();

    virtual void apply(Buffer *buffer) = 0;

    virtual void initialization(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &list);

    ///
    /// \brief generateAudioParameters
    /// \return audio parameters with AudioFormat::PCM_FLOAT
    ///
    const AudioParameters generateAudioParameters() const;

private:
    quint32                                 m_sampleRate = 0;
    QList<AudioParameters::ChannelPosition> m_list;

};


} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // AUDIOEFFECT_H
