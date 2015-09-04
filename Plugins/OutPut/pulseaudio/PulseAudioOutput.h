#ifndef PULSEAUDIOOUTPUT_H
#define PULSEAUDIOOUTPUT_H

#include "OutPut/IOutPut.h"

extern "C"{
#include <pulse/simple.h>
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
    explicit PulseAudioOutput(QObject *parent = 0);
    virtual ~PulseAudioOutput();

    // IOutPut interface
public:
    bool initialize(quint32 srate, int chan, AudioParameters::AudioFormat f);
    qint64 latency();
    qint64 writeAudio(unsigned char *data, qint64 maxSize);
    void drain();
    void reset();
private:
    void uninitialize();
    pa_simple *m_connection;

};

} //PulseAudioOutput
} //OutPut
} //PhoenixPlayer
#endif // PULSEAUDIOOUTPUT_H
