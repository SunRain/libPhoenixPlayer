#ifndef DITHERING_H
#define DITHERING_H

#include "AudioEffect.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {
    namespace PlayBackend {
        namespace PhoenixBackend {
            class Buffer;

class Dithering : public AudioEffect
{
public:
    Dithering();
    virtual ~Dithering() override;

    void setFormats(AudioParameters::AudioFormat in, AudioParameters::AudioFormat out);

    // AudioEffect interface
public:
    void apply(Buffer *buffer) Q_DECL_OVERRIDE;
    void initialization(quint32 sampleRate, const ChannelMap &map) Q_DECL_OVERRIDE;

private:
    typedef struct
    {
        float error[3];
        quint32 random;

    } AudioDither;

private:
    void clearHistory();
    quint32 prng(quint32 state);
    float audioLinearDither(float sample, AudioDither *dither);

private:
    int m_chan                  = 2;
    float m_lsb                 = false;
    bool m_required             = false;
    AudioDither m_dither[9];
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // DITHERING_H
