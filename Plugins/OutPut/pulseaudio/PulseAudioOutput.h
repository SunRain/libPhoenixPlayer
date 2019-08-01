#ifndef PULSEAUDIOOUTPUT_H
#define PULSEAUDIOOUTPUT_H

#include "OutPut/IOutPut.h"

extern "C"{
#include <pulse/pulseaudio.h>
}

namespace PhoenixPlayer {
namespace OutPut {
namespace PulseAudioOutput {

class PulseAudioOutput : public IOutPut
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.OutPut.PulseAudioOutput" FILE "pulseaudio.json")
    Q_INTERFACES(PhoenixPlayer::OutPut::IOutPut)
public:
    explicit PulseAudioOutput(QObject *parent = Q_NULLPTR);
    virtual ~PulseAudioOutput() override;

    // IOutPut interface
public:
    bool initialize(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &channels, AudioParameters::AudioFormat f) Q_DECL_OVERRIDE;
    qint64 latency() Q_DECL_OVERRIDE;
    qint64 writeAudio(unsigned char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    void drain() Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;
    void suspend() Q_DECL_OVERRIDE;
    void resume() Q_DECL_OVERRIDE;
private:
    void uninitialize();
    bool isReady() const;
    void poll();
    bool process(pa_operation *op);

private:
    pa_mainloop *m_loop     = Q_NULLPTR;
    pa_context *m_ctx       = Q_NULLPTR;
    pa_stream *m_stream     = Q_NULLPTR;
    QHash <AudioParameters::ChannelPosition, pa_channel_position_t> m_pa_channels;
};

} //PulseAudioOutput
} //OutPut
} //PhoenixPlayer
#endif // PULSEAUDIOOUTPUT_H
