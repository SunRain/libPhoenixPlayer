#include "MainWindow.h"

#include <QtGlobal>
#include <QDebug>
#include <QVBoxLayout>

#include "AudioParameters.h"
#include "PluginLoader.h"
#include "PluginHost.h"
#include "PPCommon.h"
#include "PPSettings.h"
#include "LibPhoenixPlayerMain.h"
#include "MediaResource.h"

#include "PhoenixPlayBackend.h"
#include "MediaResource.h"
#include "Backend/BaseVolume.h"
#include "PluginMgr.h"

//#include "BufferQueue.h"
//#include "StateHandler.h"
//#include "AudioConverter.h"
//#include "ChannelConverter.h"
//#include "AudioEffect.h"
//#include "Dithering.h"

using namespace PhoenixPlayer;
using namespace PhoenixPlayer::PlayBackend;
using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    PluginMgr mgr;
    m_playBackend =  qobject_cast<IPlayBackend*>(PluginMgr::instance(mgr.usedPlayBackend())); //new PhoenixPlayBackend(this);
    m_playBackend->initialize();

    MediaResource *res = MediaResource::create("/home/wangguojian/音乐/aaaa.mp3");

    m_playBackend->changeMedia(res, 0, false);

    m_playBackend->play();

    QWidget *wd = new QWidget(this);
    this->setCentralWidget(wd);


    QBoxLayout *layout = new QVBoxLayout;
    wd->setLayout(layout);

    m_infoLabel = new QLabel;
    m_infoLabel->setText("aaaaa");
    layout->addWidget(m_infoLabel);

    m_playPauseBtn = new QPushButton;
    layout->addWidget(m_playPauseBtn);
    m_playPauseBtn->setText("paly/pause");
    connect(m_playPauseBtn, &QPushButton::clicked, [&]() {
        if (m_playBackend->playBackendState() == PPCommon::PlayBackendPlaying) {
            m_playBackend->pause();
        } else if (m_playBackend->playBackendState() == PPCommon::PlayBackendPaused) {
            m_playBackend->play(m_tickSec);
        } else {
            qDebug()<<Q_FUNC_INFO<<"play state stop";
        }
    });

    m_slider = new QSlider;
    m_slider->setOrientation(Qt::Orientation::Horizontal);
    m_slider->setTracking(false);
    layout->addWidget(m_slider);
    m_slider->setMinimum(0);
    connect(m_slider, &QSlider::valueChanged, [&](int value) {
        qDebug()<<Q_FUNC_INFO<<"slider change to "<<value;
        m_playBackend->setPosition(value);
        updateLabel();
    });

    m_volume = new QSlider;
    m_volume->setMaximum(100);
    m_volume->setMinimum(0);
    m_volume->setTracking(false);
    m_volume->setOrientation(Qt::Orientation::Horizontal);
    layout->addWidget(m_volume);
    connect(m_volume, &QSlider::valueChanged, [&](int value){
        m_playBackend->baseVolume()->setVolume(value);
        m_vol = QString::number(value);
    });

    connect(m_playBackend, &IPlayBackend::stateChanged, [&](PPCommon::PlayBackendState state) {
        if (state == PPCommon::PlayBackendPlaying) {
            m_playPauseBtn->setText("click pause");
            m_playState = "playing";
        } else if (state == PPCommon::PlayBackendPaused) {
            m_playPauseBtn->setText("click play");
            m_playState = "paused";
        } else {
            m_playPauseBtn->setText("stopped");
            m_playState = "stopped";
        }
        m_slider->setMaximum(m_playBackend->durationInMS()/1000);
        m_duration = QString::number(m_playBackend->durationInMS()/1000);
        updateLabel();
    });

    connect(m_playBackend, &IPlayBackend::tickInSec, [&](quint64 tick){
        qDebug()<<Q_FUNC_INFO<<"tick sec "<<tick;
        m_tickSec = tick;
        m_playTick = QString::number(m_tickSec);
//        m_slider->setValue(m_tickSec);
        updateLabel();
    });






}

MainWindow::~MainWindow()
{

}

void MainWindow::tst_decoder()
{
#if 0
    if (m_decoder) {
        //TODO 判断当前decoder是否支持解析当前的media
    } else {
        //TODO 使用PluginLoader装载不同的Decoder，需要添加Decoder是否支持当前媒体的接口
        foreach (const QString &s, m_decoderLibs) {
            m_decoderHost = new DecoderHost(s);
            if (m_decoderHost->isValid()) { //TODO 需要判断当前decoder/host是否支持当前媒体的解码
                m_decoder = m_decoderHost->instance<IDecoder>();
                if (!m_decoder) {
                    if (!m_decoderHost->unLoad()) {
                        m_decoderHost->forceUnload();
                    }
                    m_decoder = nullptr;
                    m_decoderHost->deleteLater();
                    m_decoderHost = nullptr;
                    continue;
                }
                break;
            }
        }
    }

    if (!m_decoder) {
        qCritical()<<Q_FUNC_INFO<<"No decoder found!!";
        return;
    }

    //TODO post state to backend
    if (!m_resource->initialize()) {
        qWarning()<<Q_FUNC_INFO<<"initialize error !";
        return;
    }

    if (!m_decoder->initialize(m_resource)) {
        qWarning()<<Q_FUNC_INFO<<"invalid file format";
        //TODO: 使用pluginloader切换到另外一个支持当前格式的decoder
        return;
    }
    m_audioParameters = m_decoder->audioParameters();

    m_bitrate = m_decoder->bitrate();
    m_blockSize = Buffer::BUFFER_PERIOD * m_audioParameters.frameSize();
    m_outputSize = m_blockSize * 4;
    m_sample_size = m_audioParameters.sampleSize();
    m_output_buf = new unsigned char[m_outputSize];

    if (!m_audioConverter) {
        m_audioConverter = new AudioConverter();
    }
    m_audioConverter->setFormat(m_audioParameters.format());
    // force set format
    m_audioParameters = AudioParameters(m_audioParameters.sampleRate(),
                                        m_audioParameters.channels(),
                                        AudioParameters::PCM_FLOAT);

    if (!m_dithering) {
        m_dithering = new Dithering();
    }
    m_dithering->initialization(m_audioParameters.sampleRate(), m_audioParameters.channels());

    if (m_channelConverter) {
        delete m_channelConverter;
        m_channelConverter = Q_NULLPTR;
    }
    if (m_audioParameters.channels() != m_audioParameters.remapedChannels()) {
        qDebug()<<Q_FUNC_INFO<<"Use channel converter!!";
        m_channelConverter = new ChannelConverter(m_audioParameters.remapedChannels());
        m_channelConverter->initialization(m_audioParameters.sampleRate(),
                                           m_audioParameters.channels());
        m_audioParameters = m_channelConverter->generateAudioParameters();
    } else {
        qDebug()<<Q_FUNC_INFO<<"Not use channel converter!!";
    }

    qDebug()<<Q_FUNC_INFO<<" decoder  "<<m_decoder->audioParameters().parametersInfo()
             <<" m_audioParameters "<<m_audioParameters.parametersInfo();

    m_dithering->setFormats(m_decoder->audioParameters().format(), m_audioParameters.format());


    while (true) {
        qint64 len = m_decoder->runDecode(m_output_buf + m_output_at, m_outputSize - m_output_at);
        if (len > 0) {
            m_output_at += len;

            while (m_output_at > m_blockSize) {

                size_t sz = m_output_at < m_blockSize ? m_output_at : m_blockSize;
                size_t samples = sz / m_sample_size;

                Buffer *b = new Buffer(m_audioParameters.channels().count() * Buffer::BUFFER_PERIOD);
                b->samples = samples;
                b->rate = m_bitrate;

                qDebug()<<Q_FUNC_INFO<<" samples "<<samples<<" buffer size "<<b->size
                         <<" m_output_at "<<m_output_at<<" m_blockSize "<<m_blockSize;

                m_audioConverter->toFloat(m_output_buf, b->data, samples);
                m_dithering->apply(b);


                if (m_channelConverter) {
                    qDebug()<<Q_FUNC_INFO<<"use channelConverter";
                    m_channelConverter->apply(b);
                }


                m_output_at -= sz;
            }

        } else if (len == 0) { //decoder finish
            qDebug()<<Q_FUNC_INFO<<"--------------- finish decoder ";
            break;

        }
    }
#endif

}

void MainWindow::updateLabel()
{
    QString text = QString("state : %1").arg(m_playState);
    text += "\n";
    text += QString("tick: %1/%2").arg(m_playTick).arg(m_duration);
    text += "\n";
    text += QString("volume: %1").arg(m_vol);
    m_infoLabel->setText(text);

}
