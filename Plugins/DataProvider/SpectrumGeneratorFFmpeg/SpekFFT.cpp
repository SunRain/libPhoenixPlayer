#include "SpekFFT.h"

extern "C" {
#include <libavutil/mem.h>
#include <libavcodec/avfft.h>
}

namespace PhoenixPlayer {
namespace DataProvider {
namespace SpectrumGeneratorFFmpeg {

SpekFFT::SpekFFT(int nbits)
    : m_inputSize(1 << nbits),
    m_outputSize((1 << (nbits - 1)) + 1),
    m_outputList(m_outputSize)
{
    // FFmpeg uses various assembly optimizations which expect
    // input data to be aligned by up to 32 bytes (e.g. AVX)
    this->m_input = (float*) av_malloc(sizeof(float) * m_inputSize);

    this->m_RDFTContext = av_rdft_init(nbits, DFT_R2C);
}

SpekFFT::~SpekFFT()
{
    if (m_input) {
        av_freep(&m_input);
    }
    if (m_RDFTContext) {
        av_rdft_end(m_RDFTContext);
    }
}

void SpekFFT::execute()
{
    av_rdft_calc(m_RDFTContext, m_input);

    // Calculate magnitudes.
    int     n           = this->m_inputSize;
    float   n2          = n * n;
    m_outputList[0]     = 10.0f * log10f(m_input[0] * m_input[0] / n2);
    m_outputList[n/2]   = 10.0f * log10f(m_input[1] * m_input[1] / n2);
    for (int i = 1; i < n / 2; ++i) {
        float re        = m_input[i * 2];
        float im        = m_input[i * 2 + 1];
        m_outputList[i] = 10.0f * log10f((re * re + im * im) / n2);
    }
}


} //SpectrumGeneratorFFmpeg
} //DataProvider
} //PhoenixPlayer
