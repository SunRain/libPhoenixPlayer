#ifndef SPEKFFT_H
#define SPEKFFT_H

#include <QVector>

struct RDFTContext;

namespace PhoenixPlayer {
namespace DataProvider {
namespace SpectrumGeneratorFFmpeg {

class SpekFFT
{
public:
    explicit SpekFFT(int nbits);
    virtual ~SpekFFT();

    void execute();

    inline int getInputSize() const
    {
        return m_inputSize;
    }

    inline int getOutputSize() const
    {
        return m_outputSize;
    }

    inline void setInput(int i, float v)
    {
        m_input[i] = v;
    }

    inline float getOutput(int i) const
    {
        return m_outputList.at(i);
    }
protected:
//    float *get_input();


private:
    RDFTContext     *m_RDFTContext  = Q_NULLPTR;
    float           *m_input        = Q_NULLPTR;
    int             m_inputSize     = 0;
    int             m_outputSize    = 0;
    QVector<float>  m_outputList;
};

} //SpectrumGeneratorFFmpeg
} //DataProvider
} //PhoenixPlayer

#endif // SPEKFFT_H
