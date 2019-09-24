#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

#include "AudioParameters.h"


namespace PhoenixPlayer {
    class PluginLoader;
    class PluginHost;
    class PPSettings;
    class MediaResource;

//    namespace Decoder {
//        class IDecoder;
//        class DecoderHost;
//    }

    namespace PlayBackend {
        namespace PhoenixBackend {
            class PhoenixPlayBackend;
            class StateHandler;
            class BufferQueue;
            class AudioConverter;
            class AudioEffect;
            class ChannelConverter;
            class Dithering;
        }
    }
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void tst_decoder();


    void updateLabel();
private:
    QLabel *m_infoLabel = Q_NULLPTR;
    QPushButton *m_playPauseBtn = Q_NULLPTR;
    QSlider *m_slider = Q_NULLPTR;
    QSlider *m_volume = Q_NULLPTR;


    PhoenixPlayer::PlayBackend::PhoenixBackend::PhoenixPlayBackend *m_playBackend = Q_NULLPTR;

    PhoenixPlayer::PlayBackend::PhoenixBackend::AudioConverter              *m_audioConverter   = Q_NULLPTR;
    PhoenixPlayer::PlayBackend::PhoenixBackend::ChannelConverter            *m_channelConverter = Q_NULLPTR;
    PhoenixPlayer::PlayBackend::PhoenixBackend::Dithering                   *m_dithering        = Q_NULLPTR;

    PhoenixPlayer::PPSettings                  *m_settings     = Q_NULLPTR;
    PhoenixPlayer::PluginLoader                *m_pluginLoader = Q_NULLPTR;
    PhoenixPlayer::MediaResource               *m_resource     = Q_NULLPTR;

//    PhoenixPlayer::Decoder::IDecoder           *m_decoder      = Q_NULLPTR;
//    PhoenixPlayer::Decoder::DecoderHost        *m_decoderHost  = Q_NULLPTR;

        QStringList m_decoderLibs;

//        PhoenixPlayer::AudioParameters m_audioParameters;
        quint64 m_tickSec = 0;

        uint m_blockSize = 0;
        uint m_sample_size = 0;
        int m_bitrate = 0;
        unsigned char *m_output_buf = Q_NULLPTR;
        quint64 m_outputSize = 0;
        quint64 m_output_at = 0;

        QByteArray m_dataArray;

        QString m_playState;
        QString m_playTick;
        QString m_duration;
        QString m_vol;

};

#endif // MAINWINDOW_H
