#ifndef CHANNELCONVERTER_H
#define CHANNELCONVERTER_H

#include "AudioEffect.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {
    class Buffer;

    namespace PlayBackend {

        namespace PhoenixBackend {

class ChannelConverter : public AudioEffect
{
public:
    ChannelConverter(const QList<AudioParameters::ChannelPosition> &outList);
    virtual ~ChannelConverter() override;

    // AudioEffect interface
public:
    void apply(Buffer *buffer) Q_DECL_OVERRIDE;
    void initialization(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &in) Q_DECL_OVERRIDE;

private:
    bool m_disabled            = true;
    int m_reorderArray[9]      = { 0 };
    float *m_tmpBuf            = nullptr;
    size_t m_tmpSize           = 0;

    QList<AudioParameters::ChannelPosition> m_inList;
    QList<AudioParameters::ChannelPosition> m_outList;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // CHANNELCONVERTER_H
