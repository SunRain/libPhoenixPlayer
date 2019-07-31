#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

#include <QtGlobal>
#include <QSharedDataPointer>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class AudioParametersPriv;
class LIBPHOENIXPLAYER_EXPORT AudioParameters
{
public:
    //Audio formats
    enum AudioFormat {
        PCM_S8 = 0x200,     /*!< Signed 8 bit */
        PCM_U8,     /*!< Unsigned 8 bit */
        PCM_S16LE,  /*!< Signed 16 bit Little Endian */
        PCM_S16BE,  /*!< Signed 16 bit Big Endian */
        PCM_U16LE,  /*!< Unsigned 16 bit Little Endian */
        PCM_U16BE,  /*!< Unsigned 16 bit Big Endian */
        PCM_S24LE,  /*!< Signed 24 bit Little Endian using low three bytes in 32-bit word */
        PCM_S24BE,  /*!< Signed 24 bit Big Endian using low three bytes in 32-bit word */
        PCM_U24LE,  /*!< Unsigned 24 bit Little Endian using low three bytes in 32-bit word */
        PCM_U24BE,  /*!< Unsigned 24 bit Big Endian using low three bytes in 32-bit word */
        PCM_S32LE,  /*!< Signed 32 bit Little Endian */
        PCM_S32BE,  /*!< Signed 32 bit Big Endian */
        PCM_U32LE,  /*!< Unsigned 32 bit Little Endian */
        PCM_U32BE,  /*!< Unsigned 32 bit Big Endian */
        PCM_FLOAT,   /*!< Float 32 bit Native Endian, range: -1.0 to 1.0 */
        PCM_UNKNOWN         //Unknown format
    };
    //Audio channels enum.
    enum ChannelPosition
    {
        CHAN_NULL         = 0x00,   /*!< No channel */
        CHAN_FRONT_LEFT   = 0x01,   /*!< Front left channel */
        CHAN_FRONT_RIGHT  = 0x02,   /*!< Front right channel */
        CHAN_REAR_LEFT    = 0x04,   /*!< Rear left channel */
        CHAN_REAR_RIGHT   = 0x08,   /*!< Rear right channel */
        CHAN_FRONT_CENTER = 0x10,   /*!< Front center channel */
        CHAN_REAR_CENTER  = 0x20,   /*!< Rear center channel */
        CHAN_SIDE_LEFT    = 0x40,   /*!< Side left channel */
        CHAN_SIDE_RIGHT   = 0x80,   /*!< Side right channel */
        CHAN_LFE          = 0x100,  /*!< Low-frequency effects channel */
    };

    //Byte order of samples.
    enum ByteOrder
    {
        LittleEndian = 0, /*!< Samples are in little-endian byte order */
        BigEndian         /*!< Samples are in big-endian byte order */
    };
public:
    AudioParameters(quint32 sampleRate = 48000, int channels = 2, AudioFormat f= AudioParameters::PCM_UNKNOWN);
    AudioParameters(quint32 sampleRate, const QList<ChannelPosition> &channels, AudioFormat f);
    AudioParameters(const AudioParameters &other);
    AudioParameters &operator =(const AudioParameters &other);
    bool operator == (const AudioParameters &other);
    bool operator != (const AudioParameters &other) {
        return !operator == (other);
    }

    quint32 sampleRate() const;

    ///
    /// \brief channels Channels from class construction
    /// \return
    ///
    const QList<ChannelPosition> channels() const;

    ///
    /// \brief remapedChannels add CHAN_NULL to channels from construction to make channel list size of 9
    /// \return
    ///
    const QList<ChannelPosition> remapedChannels() const;

    AudioFormat format() const;

    int sampleSize() const;

    ///
    /// Returns the number of bytes required to represent one frame
    /// (a sample in each channel) in this format.
    ///
    int frameSize() const;

    int bitsPerSample() const;

    ByteOrder byteOrder() const;

    //Returns sample size in bytes of the given pcm data format.
    static int sampleSize(AudioFormat format);

    ///
    /// \brief validBitsPerSample
    /// \param format
    /// \return  Returns the number of used bits in the sample.
    /// The value shoud be less or equal to the value returned by \b bitsPerSample().
    ///
    static int validBitsPerSample(AudioFormat format);

    void printInfo();
    QString parametersInfo() const;
private:
    void generateMap(int channels);

private:
    QSharedDataPointer<PhoenixPlayer::AudioParametersPriv> d;
};

class AudioParametersPriv : public QSharedData
{
public:
    AudioParametersPriv() {}
    quint32 sampleRate = 48000;
    AudioParameters::AudioFormat format = AudioParameters::PCM_UNKNOWN;
    int sampleSize = 2;
    int validBitsPerSample = 16;
    QList<AudioParameters::ChannelPosition> channelList;
};

} //PhoenixPlayer
#endif // AUDIOPARAMETERS_H
