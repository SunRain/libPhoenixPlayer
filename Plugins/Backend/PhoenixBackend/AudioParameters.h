#ifndef AUDIOPARAMETERS_H
#define AUDIOPARAMETERS_H

#include <QtGlobal>
#include <QSharedDataPointer>

#include "libphoenixplayer_global.h"

#include "ChannelMap.h"

namespace PhoenixPlayer {

    namespace PlayBackend {

        namespace PhoenixBackend {

class AudioParametersPriv;
class AudioParameters
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

    //Byte order of samples.
    enum ByteOrder
    {
        LittleEndian = 0, /*!< Samples are in little-endian byte order */
        BigEndian         /*!< Samples are in big-endian byte order */
    };
public:
//    AudioParameters(quint32 sampleRate = 48000, int channels = 2, AudioFormat f= AudioParameters::PCM_UNKNOWN);

//    AudioParameters(quint32 sampleRate, const QList<ChannelPosition> &channels, AudioFormat f);

    AudioParameters();

    AudioParameters(quint32 sampleRate, const ChannelMap &map, AudioFormat f);

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
//    const QList<ChannelPosition> channels() const;

    int channels() const;


    const ChannelMap channelMap() const;


    ///
    /// \brief remapedChannels add CHAN_NULL to channels from construction to make channel list size of 9
    /// \return
    ///
//    const QList<ChannelPosition> remapedChannels() const;

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
//    void generateMap(int channels);

private:
    QSharedDataPointer<AudioParametersPriv> d;
};

class AudioParametersPriv : public QSharedData
{
public:
    AudioParametersPriv() {}
    quint32 sampleRate = 0;
    AudioParameters::AudioFormat format = AudioParameters::PCM_S16LE;
    int sampleSize = 2;
    int validBitsPerSample = 16;
    ChannelMap channelMap;
};

} //PhoenixPlayer
} //PlayBackend
} //PhoenixPlayer

#endif // AUDIOPARAMETERS_H
