#ifndef QTMULTIMEDIAOUTPUT_H
#define QTMULTIMEDIAOUTPUT_H

#include "IOutPut.h"
#include <QScopedPointer>

class QIODevice;
class QAudioOutput;
namespace PhoenixPlayer {

    namespace PlayBackend {

        namespace PhoenixBackend {

            namespace OutPut {

                namespace QtMultimediaOutput {

class OutputControl;
class QtMultimediaOutput : public IOutPut
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "PhoenixPlayer.OutPut.QtMultimediaOutput" FILE "QtAudio.json")
//    Q_INTERFACES(PhoenixPlayer::OutPut::IOutPut)
public:
    explicit QtMultimediaOutput(QObject *parent = nullptr);
    virtual ~QtMultimediaOutput() override;

    // IOutPut interface
public:
    bool initialize(quint32 sampleRate,
//                    const QList<AudioParameters::ChannelPosition> &channels,
                    const ChannelMap &map,
                    AudioParameters::AudioFormat f) Q_DECL_OVERRIDE;

    qint64 latency() Q_DECL_OVERRIDE;
    qint64 writeAudio(unsigned char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    void drain() Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;
    void suspend() Q_DECL_OVERRIDE;
    void resume() Q_DECL_OVERRIDE;

private:
    QScopedPointer<QAudioOutput> m_output;
    QScopedPointer<OutputControl> m_control;
    QIODevice *m_buffer;
    qint64 m_bytes_per_second;
};

class OutputControl : public QObject
{
    Q_OBJECT

public:
    OutputControl(QAudioOutput *o);

public slots:
    void suspend();
    void resume();

private:
    QAudioOutput *m_output;

};

} //QtMultimediaOutput
} //OutPut
} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // QTMULTIMEDIAOUTPUT_H
