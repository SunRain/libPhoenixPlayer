#ifndef IOUTPUT_H
#define IOUTPUT_H

#include <QObject>
#include "AudioParameters.h"

namespace PhoenixPlayer {
    class Buffer;

    namespace OutPut {

class IOutPut : public QObject
{
    Q_OBJECT
public:
    explicit IOutPut(QObject *parent = Q_NULLPTR);
    virtual ~IOutPut();

    ///
    /// \brief initialize
    /// call this when reimplement this function.
    /// \param sampleRate
    /// \param channels
    /// \param f
    /// \return
    ///
    virtual bool initialize(quint32 sampleRate,
                            const QList<AudioParameters::ChannelPosition> &channels,
                            AudioParameters::AudioFormat f);
    /*
     * Returns output interface latency in milliseconds.
     */
    virtual qint64 latency() = 0;

    /*
     * Writes up to maxSize bytes from  data to the output interface device.
     * Returns the number of bytes written, or -1 if an error occurred.
     * Subclass should reimplement this function.
     */
    virtual qint64 writeAudio(unsigned char *data, qint64 maxSize) = 0;
    /*
     * Writes all remaining plugin's internal data to audio output device.
     * Subclass should reimplement this function.
     */
    virtual void drain() = 0;
    /*
     * Drops all plugin's internal data, resets audio device
     * Subclass should reimplement this function.
     */
    virtual void reset() = 0;
    /*
     * Stops processing audio data, preserving buffered audio data.
     */
    virtual void suspend() {}
    /*
     * Resumes processing audio data.
     */
    virtual void resume() {}

    ///
    /// \brief audioParameters
    /// \return selected audio parameters
    ///
    AudioParameters audioParameters() const;

    quint32 sampleRate() const;

    QList<AudioParameters::ChannelPosition> channels() const;

    AudioParameters::AudioFormat format() const;

    int sampleSize() const;

private:
    int                                     m_sampleSize    = 0;
    quint32                                 m_sampleRate    = 0;
    AudioParameters::AudioFormat            m_format        = AudioParameters::PCM_UNKNOWN;
    QList<AudioParameters::ChannelPosition> m_channels;

};
} //Decoder
} //PhoenixPlayer
Q_DECLARE_INTERFACE(PhoenixPlayer::OutPut::IOutPut, "PhoenixPlayer.OutPut.BaseOutPutInterface/1.0")
#endif // IOUTPUT_H
