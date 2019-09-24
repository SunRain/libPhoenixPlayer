#ifndef AUDIOCONVERTER_H
#define AUDIOCONVERTER_H

#include "AudioParameters.h"

namespace PhoenixPlayer {
    namespace PlayBackend {
        namespace PhoenixBackend {

class AudioConverter
{
public:
    AudioConverter();
    virtual ~AudioConverter();

    void setFormat(AudioParameters::AudioFormat f);

    ///
    /// \brief toFloat Converts raw samples to AudioParameters::PCM_FLOAT
    /// \param in
    /// \param out
    /// \param samples Number of samples
    ///
    void toFloat(const unsigned char *in, float *out, size_t samples);

    ///
    /// \brief fromFloat Converts samples from AudioParameters::PCM_FLOAT to specified working format
    /// \param in
    /// \param out
    /// \param samples Number of samples
    ///
    void fromFloat(const float *in, const unsigned char *out, size_t samples);
private:
    AudioParameters::AudioFormat m_format = AudioParameters::PCM_UNKNOWN;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // AUDIOCONVERTER_H
