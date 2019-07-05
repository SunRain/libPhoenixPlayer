#include "SpectrumGeneratorFFmpeg.h"

#include <QTimer>
#include <QEventLoop>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>

#include "qt-spek/spek-pipeline.h"
#include "qt-spek/spek-audio.h"
#include "qt-spek/spek-fft.h"
#include "qt-spek/spek-ruler.h"

//using namespace PhoenixPlayer::MusicLibrary;
using namespace PhoenixPlayer::MusicLibrary::SpectrumGeneratorFFmpeg;

const static int FFT_BITS           = 11;

//FIXME what is this?
const static int URANGE             = 0;
const static int LRANGE             = -120;
const static int SAMPLE_LENGTH      =  500;

static void pipeline_cb(int bands, int sample, float *values, void *cb_data);
static void pipline_fcb(void *fcb_data);

namespace PhoenixPlayer {
namespace MusicLibrary {
namespace SpectrumGeneratorFFmpeg {
class SpekSpectrogram
{
public:
    explicit SpekSpectrogram(SpectrumGeneratorFFmpeg *sg)
        : m_sg(sg),
        audio(new Audio),
        fft(new FFT())
    {

    }
    ~SpekSpectrogram()
    {
        stop();
    }
    void open(const QString &file)
    {
        this->path = file;
        dataList.clear();
        start();
    }
    void stop()
    {
        if (this->pipeline) {
            spek_pipeline_close(this->pipeline);
            this->pipeline = Q_NULLPTR;
        }
        finishFlag = true;
    }
    bool finishFlag = false;
    QString path;
    QList<QList<qreal> > dataList;
private:
    void start()
    {
        if (path.isEmpty()) {
            return;
        }
        this->stop();

        finishFlag = false;
        // The number of samples is the number of pixels available for the image.
        // The number of bands is fixed, FFT results are very different for
        // different values but we need some consistency.
        //        int samples = /*m_slider->width()*/ SAMPLE_LENGTH /(m_slider->waveWidth() + m_slider->waveSpacing());
        int samples = SAMPLE_LENGTH;
        if (samples > 0) {
            this->pipeline = spek_pipeline_open(
                this->audio->open(std::string(this->path.toUtf8().data()), 0),
                this->fft->create(FFT_BITS),
                0,
                0,
                window_function::WINDOW_DEFAULT,
                samples,
                pipeline_cb,
                this,
                pipline_fcb,
                this
                );
            spek_pipeline_start(this->pipeline);
        }
    }
private:
    SpectrumGeneratorFFmpeg *m_sg = Q_NULLPTR;
    spek_pipeline *pipeline = Q_NULLPTR;
    std::unique_ptr<Audio> audio;
    std::unique_ptr<FFT> fft;
};

} //SpectrumGeneratorFFmpeg
} //MusicLibrary
} //PhoenixPlayer

static void pipeline_cb(int bands, int sample, float *values, void *cb_data)
{
    SpekSpectrogram *spek = static_cast<SpekSpectrogram*>(cb_data);
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
                foreach (const auto i, subList) {
                    value += i;
                }
                list.append(value);
                subList.clear();
            }
        } else {
            list.append(level);
        }
    }
    spek->dataList.append(list);
}

static void pipline_fcb(void *fcb_data) {
    SpekSpectrogram *spek = static_cast<SpekSpectrogram*>(fcb_data);
    spek->finishFlag = true;
}


SpectrumGeneratorFFmpeg::SpectrumGeneratorFFmpeg(QObject *parent)
    : ISpectrumGenerator (parent)
{
    m_spek = new SpekSpectrogram(this);
}

SpectrumGeneratorFFmpeg::~SpectrumGeneratorFFmpeg()
{
    m_spek->stop();
    delete m_spek;
    m_spek = Q_NULLPTR;
}

QList<QList<qreal> > SpectrumGeneratorFFmpeg::generate(const PhoenixPlayer::AudioMetaObject &obj) const
{
    const QUrl url = obj.uri();
    QString file;
    if (!url.scheme().isEmpty()) {
        file = obj.uri().toLocalFile();
    } else {
        file = url.toString();
    }
    if (!QFile::exists(file)) {
        qWarning()<<Q_FUNC_INFO<<"Can't find file "<<file;
        return QList<QList<qreal> >();
    }
    m_spek->stop();
    m_spek->open(file);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(false);
    timer.setInterval(30);
    connect(&timer, &QTimer::timeout,
             this, [&]() {
                if (m_spek->finishFlag) {
                    timer.stop();
                    loop.quit();
                }
                qApp->processEvents();
             });
    timer.start(30);
    loop.exec();
    return m_spek->dataList;
}
