#ifndef CHANNELCONVERTER_H
#define CHANNELCONVERTER_H

#include "AudioEffect.h"
#include "AudioParameters.h"

namespace PhoenixPlayer {
    namespace PlayBackend {
        namespace PhoenixBackend {
        class Buffer;

class ChannelConverter : public AudioEffect
{
public:
    ChannelConverter(const ChannelMap &out_map);

    virtual ~ChannelConverter() override;

    // AudioEffect interface
public:
    void apply(Buffer *buffer) Q_DECL_OVERRIDE;
    void initialization(quint32 sampleRate, const ChannelMap &in_map) Q_DECL_OVERRIDE;

private:
    bool        m_disabled          = true;
    int         m_reorderArray[9]   = { 0 };
    float       *m_tmpBuf           = nullptr;
    size_t      m_tmpSize           = 0;

    ChannelMap  m_out_map;
    ChannelMap  m_in_map;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // CHANNELCONVERTER_H
