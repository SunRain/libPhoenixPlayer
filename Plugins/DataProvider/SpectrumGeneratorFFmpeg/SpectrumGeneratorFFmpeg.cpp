#include "SpectrumGeneratorFFmpeg.h"

#include <QCoreApplication>
#include <QTimer>
#include <QEventLoop>
#include <QCoreApplication>
#include <QFile>

#include <Logger.h>

#include "SpekAudio.h"
#include "SpekFFT.h"

namespace PhoenixPlayer {
namespace DataProvider {
namespace SpectrumGeneratorFFmpeg {

typedef  QList<QList<qreal> > SpekVauleList;

const static int FFT_BITS           = 11;
const static int NFFT = 64;

//FIXME what is this?
const static int URANGE             = 0;
const static int LRANGE             = -120;
const static int SAMPLE_LENGTH      =  500;

enum window_function
{
    WINDOW_HANN,
    WINDOW_HAMMING,
    WINDOW_BLACKMAN_HARRIS,
    WINDOW_COUNT,
    WINDOW_DEFAULT = WINDOW_HANN,
};

static float get_window(enum window_function f, int i, float *coss, int n) {
    switch (f) {
    case WINDOW_HANN:
        return 0.5f * (1.0f - coss[i]);
    case WINDOW_HAMMING:
        return 0.53836f - 0.46164f * coss[i];
    case WINDOW_BLACKMAN_HARRIS:
        return 0.35875f - 0.48829f * coss[i] + 0.14128f * coss[2*i % n] - 0.01168f * coss[3*i % n];
    default:
        assert(false);
        return 0.0f;
    }
}

class SpectrumGeneratorFFmpegInternal
{
public:
    explicit SpectrumGeneratorFFmpegInternal() {}
    ~SpectrumGeneratorFFmpegInternal() {}

    float *coss; // Pre-computed cos table.
    float *input;
    float *output;
    int stream;
    int channel;
    int samples;
    int nfft = 0; // Size of the FFT transform.
    int input_size;
    int input_pos;
    bool stop = false;

    SpekVauleList  spekVauleList;
};

SpectrumGeneratorFFmpeg::SpectrumGeneratorFFmpeg(QObject *parent)
    : ISpectrumGenerator (parent)
{
    m_spekAudio = new SpekAudio();
    m_spekFFT = new SpekFFT(FFT_BITS);
    m_internal = new SpectrumGeneratorFFmpegInternal();
}

SpectrumGeneratorFFmpeg::~SpectrumGeneratorFFmpeg()
{
    delete m_internal;
    delete m_spekAudio;
    delete m_spekFFT;
}

PluginProperty SpectrumGeneratorFFmpeg::property() const
{
    return PluginProperty("SpectrumGeneratorFFmpeg",
                          "1.0",
                          "Spectrum generator based on qt-spek",
                          false,
                          false);
}

void SpectrumGeneratorFFmpeg::stop()
{
    m_internal->stop = true;
}

QList<QList<qreal> > SpectrumGeneratorFFmpeg::generate(const AudioMetaObject &obj) const
{
    if (obj.isHashEmpty()) {
        return SpekVauleList();
    }

    const QString uri = obj.uri().toLocalFile();
    if (!QFile::exists(uri)) {
        LOG_WARNING()<<"wrong file path ["<<uri<<"]";
        return SpekVauleList();
    }

    SpekAudio::AudioError error = m_spekAudio->open(uri, 0);
    if (error != SpekAudio::AudioError::OK) {
        return SpekVauleList();
    }

    if (m_spekAudio->getError() != SpekAudio::AudioError::OK) {
        return SpekVauleList();
    }

    m_internal->nfft = m_spekFFT->getInputSize();
    m_internal->coss = (float*)malloc(m_internal->nfft * sizeof(float));
    float cf = 2.0f * (float)M_PI / (m_internal->nfft - 1.0f);
    for (int i = 0; i < m_internal->nfft; ++i) {
        m_internal->coss[i] = cosf(cf * i);
    }
    m_internal->input_size = m_internal->nfft * (NFFT * 2 + 1);
    m_internal->input = (float*)malloc(m_internal->input_size * sizeof(float));
    m_internal->output = (float*)malloc(m_spekFFT->getOutputSize() * sizeof(float));

    error = m_spekAudio->start(0, SAMPLE_LENGTH);

    if (error  != SpekAudio::AudioError::OK) {
        return SpekVauleList();
    }

    m_internal->input_pos = 0;
    m_internal->stop = false;

    readSpekAudio();

    return m_internal->spekVauleList;
}

void SpectrumGeneratorFFmpeg::readSpekAudio() const
{
    int pos = 0, prev_pos = 0;
    int len;

    while ((len = m_spekAudio->read()) > 0) {

        qApp->processEvents();

        if (m_internal->stop) {
            break;
        }
        const float *buffer = m_spekAudio->getBuffer();
        while (len-- > 0) {
            qApp->processEvents();

            m_internal->input[pos] = *buffer++;
            pos = (pos + 1) % m_internal->input_size;

            // Wake up the worker if we have enough data.
            if ((pos > prev_pos ? pos : pos + m_internal->input_size) - prev_pos == m_internal->nfft * NFFT) {
                prev_pos = pos;
                m_internal->input_pos = prev_pos;
                processData();
            }
        }
        assert(len == -1);
    }

    if (pos != prev_pos) {
        m_internal->input_pos = prev_pos;
        processData();
    }
    decorateSpek(m_spekFFT->getOutputSize(), -1, Q_NULLPTR);
}

void SpectrumGeneratorFFmpeg::processData() const
{
    int sample = 0;
    int64_t frames = 0;
    int64_t num_fft = 0;
    int64_t acc_error = 0;
    int head = 0, tail = 0;
    int prev_head = 0;

    memset(m_internal->output, 0, sizeof(float) * m_spekFFT->getOutputSize());

    while (true) {
        if (tail == m_internal->input_pos) {
            break;
        }
        tail = m_internal->input_pos;

        if (tail == -1) {
            break;
        }

        while (true) {

            qApp->processEvents();

            head = (head + 1) % m_internal->input_size;
            if (head == tail) {
                head = prev_head;
                break;
            }
            frames++;

            // If we have enough frames for an FFT or we have
            // all frames required for the interval run and FFT.
            bool int_full =
                acc_error < m_spekAudio->getErrorBase() &&
                frames == m_spekAudio->getFramesPerInterval();
            bool int_over =
                acc_error >= m_spekAudio->getErrorBase() &&
                frames == 1 + m_spekAudio->getFramesPerInterval();

            if (frames % m_internal->nfft == 0 || ((int_full || int_over) && num_fft == 0)) {
                prev_head = head;
                for (int i = 0; i < m_internal->nfft; ++i) {
                    float val = m_internal->input[(m_internal->input_size + head - m_internal->nfft + i) % m_internal->input_size];
                    val *= get_window(WINDOW_DEFAULT, i, m_internal->coss, m_internal->nfft);
                    m_spekFFT->setInput(i, val);
                }
                m_spekFFT->execute();
                num_fft++;
                for (int i = 0; i < m_spekFFT->getOutputSize(); i++) {
                    m_internal->output[i] += m_spekFFT->getOutput(i);
                }
            }

            // Do we have the FFTs for one interval?
            if (int_full || int_over) {
                if (int_over) {
                    acc_error -= m_spekAudio->getErrorBase();
                } else {
                    acc_error += m_spekAudio->getErrorPerInterval();
                }

                for (int i = 0; i < m_spekFFT->getOutputSize(); i++) {
                    m_internal->output[i] /= num_fft;
                }

                if (sample == m_internal->samples) {
                    break;
                }

                this->decorateSpek(m_spekFFT->getOutputSize(), sample++, m_internal->output);

                memset(m_internal->output, 0, sizeof(float) * m_spekFFT->getOutputSize());
                frames = 0;
                num_fft = 0;
            }
        }
    }
}

void SpectrumGeneratorFFmpeg::decorateSpek(int bands, int sample, float *values) const
{
    if (sample == -1) {
        return;
    }
    double range = URANGE - LRANGE;
    QList<qreal> list;
    QList<qreal> subList;
    int subSize = 0;
    if (bands > 100) {
        int step = bands / 100;
        subSize = bands / step;
    }
    for (int y = 0; y < bands; y++) {
        double value = fmin(URANGE, fmax(LRANGE, values[y]));
        double level = (value - LRANGE) / range;
        if (subSize > 0) {
            subList.append(level);
            if (subList.size() >= subSize) {
                qreal value = 0;
                foreach (const auto &i, subList) {
                    value += i;
                }
                list.append(value);
                subList.clear();
            }
        } else {
            list.append(level);
        }
    }
    m_internal->spekVauleList.append(list);
}

} //SpectrumGeneratorFFmpeg
} //DataProvider
} //PhoenixPlayer
