#include "Dithering.h"

#include <QDebug>

#include "Buffer.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

Dithering::Dithering()
{
    clearHistory();
}

Dithering::~Dithering()
{

}

void Dithering::setFormats(AudioParameters::AudioFormat in, AudioParameters::AudioFormat out)
{
    m_required = false;
    if (AudioParameters::sampleSize(in) > AudioParameters::sampleSize(out)) {
        switch (out)
        {
        case AudioParameters::PCM_S8:
        case AudioParameters::PCM_U8:
            m_lsb = 1.0f / 0x80;
            m_required = true;
            break;
        case AudioParameters::PCM_S16LE:
        case AudioParameters::PCM_S16BE:
        case AudioParameters::PCM_U16LE:
        case AudioParameters::PCM_U16BE:
            m_lsb = 1.0f / 0x8000;
            m_required = true;
            break;
        default:
            ;
        }
    }

    qDebug()<<Q_FUNC_INFO<<"Dithering enabled : "<<m_required;
}

void Dithering::apply(Buffer *buffer)
{
    if (m_required) {
        for (size_t i = 0; i < buffer->samples; ++i) {
            buffer->data[i] = audioLinearDither(buffer->data[i], &m_dither[i % m_chan]);
        }
    }
}

void Dithering::initialization(quint32 sampleRate, const ChannelMap &map)
{
    AudioEffect::initialization(sampleRate, map);
    m_chan = map.count();
    m_required = false;
    clearHistory();
}

//void Dithering::initialization(quint32 sampleRate, const QList<PhoenixPlayer::AudioParameters::ChannelPosition> &list)
//{
//    AudioEffect::initialization(sampleRate, list);
//    m_chan = list.count();
//    m_required = false;
//    clearHistory();
//}

void Dithering::clearHistory()
{
    for (int i = 0; i < 9; ++i) {
        m_dither[i].error[0] = 0.0f;
        m_dither[i].error[1] = 0.0f;
        m_dither[i].error[2] = 0.0f;
        m_dither[i].random = 0;
    }
}

quint32 Dithering::prng(quint32 state) // 32-bit pseudo-random number generator
{
    return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

float Dithering::audioLinearDither(float sample, Dithering::AudioDither *dither)
{
    float output;
    quint32 random;

    /* noise shape */
    sample += dither->error[0] - dither->error[1] + dither->error[2];

    dither->error[2] = dither->error[1];
    dither->error[1] = dither->error[0] / 2;

    /* bias */
    output = sample + m_lsb;

    /* dither */
    random = prng(dither->random);
    output += (float)(random - dither->random) / 0xffffffffL * m_lsb;
    dither->random = random;

    /* clip */
    if (output > 1.0f) {
        output = 1.0f;

        if (sample > 1.0f) {
            sample = 1.0f;
        }
    } else if (output < -1.0f) {
        output = -1.0f;

        if (sample < -1.0f) {
            sample = -1.0f;
        }
    }

    /* error feedback */
    dither->error[0] = sample - output;

    return output;
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

