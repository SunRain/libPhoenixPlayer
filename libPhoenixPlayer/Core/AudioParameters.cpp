#include "AudioParameters.h"

#include <QDebug>
#include <QSharedData>

namespace PhoenixPlayer {

//class AudioParametersPriv : public QSharedData
//{
//public:
//    AudioParametersPriv() {}
//    quint32 sampleRate = 48000;
//    AudioParameters::AudioFormat format = AudioParameters::PCM_UNKNOWN;
//    int sampleSize = 2;
//    int validBitsPerSample = 16;
//    QList<AudioParameters::ChannelPosition> channelList;
//};

static AudioParameters::ChannelPosition S_INTERNAL_MAP[9] = {
    AudioParameters::ChannelPosition::CHAN_FRONT_LEFT,
    AudioParameters::ChannelPosition::CHAN_FRONT_RIGHT,
    AudioParameters::ChannelPosition::CHAN_REAR_LEFT,
    AudioParameters::ChannelPosition::CHAN_REAR_RIGHT,
    AudioParameters::ChannelPosition::CHAN_FRONT_CENTER,
    AudioParameters::ChannelPosition::CHAN_REAR_CENTER,
    AudioParameters::ChannelPosition::CHAN_LFE,
    AudioParameters::ChannelPosition::CHAN_SIDE_LEFT,
    AudioParameters::ChannelPosition::CHAN_SIDE_RIGHT
};


AudioParameters::AudioParameters(quint32 srate, int chans, AudioParameters::AudioFormat f)
    : d(new AudioParametersPriv())
{
    generateMap(chans);
    d.data()->sampleRate = srate;
    d.data()->format = f;
    d.data()->sampleSize = sampleSize(f);
    d.data()->validBitsPerSample = validBitsPerSample(f);
}

AudioParameters::AudioParameters(quint32 sampleRate,
                                 const QList<AudioParameters::ChannelPosition> &channels,
                                 AudioParameters::AudioFormat f)
    : d (new AudioParametersPriv())
{
    d.data()->channelList = channels;
    d.data()->sampleRate = sampleRate;
    d.data()->format = f;
    d.data()->sampleSize = sampleSize(f);
    d.data()->validBitsPerSample = validBitsPerSample(f);
}

AudioParameters::AudioParameters(const AudioParameters &other)
    : d(other.d)
{

}

AudioParameters &AudioParameters::operator =(const AudioParameters &other) {
    if (this != &other)
        d.operator = (other.d);
    return *this;
}

bool AudioParameters::operator ==(const AudioParameters &other)
{
    return d.data()->channelList == other.d.data()->channelList
            && d.data ()->format == other.d.data ()->format
            && d.data ()->sampleRate == other.d.data ()->sampleRate
//            && d.data()->sampleSize == other.d.data()->sampleSize
            && d.data()->validBitsPerSample == other.d.data()->validBitsPerSample;
}

quint32 AudioParameters::sampleRate() const
{
    return d.data ()->sampleRate;
}

//void AudioParameters::setSampleRate(quint32 srate)
//{
//    d.data ()->srate = srate;
//}

const QList<AudioParameters::ChannelPosition> AudioParameters::channels() const
{
    return d.data()->channelList;
}

const QList<AudioParameters::ChannelPosition> AudioParameters::remapedChannels() const
{
    QList<ChannelPosition> list;
    for(int i = 0; i < 9; ++i) {
        if(d.data()->channelList.contains(S_INTERNAL_MAP[i])) {
            list.append(S_INTERNAL_MAP[i]);
        }
    }
    while (list.count() < d.data()->channelList.count()) {
        list.append(ChannelPosition::CHAN_NULL);
    }
    return list;
}

//quint32 AudioParameters::channels() const
//{
//    return d.data ()->chan;
//}

//void AudioParameters::setChannels(quint32 chan)
//{
//    d.data ()->chan = chan;
//}

AudioParameters::AudioFormat AudioParameters::format() const
{
    return d.data ()->format;
}

//void AudioParameters::setFormat(AudioParameters::AudioFormat f)
//{
//    d.data ()->format = f;
//}


int AudioParameters::sampleSize() const
{
    return d.data()->sampleSize;
}

int AudioParameters::frameSize() const
{
    return d.data()->sampleSize * d.data()->channelList.size();
}

int AudioParameters::bitsPerSample() const
{
    return d.data()->sampleSize * 8;
}

AudioParameters::ByteOrder AudioParameters::byteOrder() const
{
    switch(d.data()->format) {
    case PCM_S16BE:
    case PCM_S24BE:
    case PCM_S32BE:
        return BigEndian;
    default:
        return LittleEndian;
    }
}

int AudioParameters::sampleSize(AudioParameters::AudioFormat format)
{
    switch(format) {
    case PCM_S8:
    case PCM_U8:
        return 1;
    case PCM_S16LE:
    case PCM_S16BE:
    case PCM_U16LE:
    case PCM_U16BE:
        return 2;
    case PCM_S24LE:
    case PCM_S24BE:
    case PCM_U24LE:
    case PCM_U24BE:
    case PCM_S32LE:
    case PCM_S32BE:
    case PCM_U32LE:
    case PCM_U32BE:
    case PCM_FLOAT:
        return 4;
    default:
        return 0;
    }
}

int AudioParameters::validBitsPerSample(AudioParameters::AudioFormat format)
{
    switch(format) {
    case PCM_S8:
    case PCM_U8:
        return 8;
    case PCM_S16LE:
    case PCM_S16BE:
    case PCM_U16LE:
    case PCM_U16BE:
        return 16;
    case PCM_S24LE:
    case PCM_S24BE:
    case PCM_U24LE:
    case PCM_U24BE:
        return 24;
    case PCM_S32LE:
    case PCM_S32BE:
    case PCM_U32LE:
    case PCM_U32BE:
    case PCM_FLOAT:
        return 32;
    default:
        return 0;
    }
}

void AudioParameters::printInfo()
{
    qDebug()<<Q_FUNC_INFO<<parametersInfo ();
}

QString AudioParameters::parametersInfo() const
{
    QHash<AudioFormat, QString> fnames;
    fnames.insert(PCM_S8, "S8");
    fnames.insert(PCM_U8, "u8");
    fnames.insert(PCM_S16LE, "s16le");
    fnames.insert(PCM_S16BE, "s16be");
    fnames.insert(PCM_U16LE, "u16le");
    fnames.insert(PCM_U16BE, "u16be");
    fnames.insert(PCM_S24LE, "s24le");
    fnames.insert(PCM_S24BE, "s24be");
    fnames.insert(PCM_U24LE, "u24le");
    fnames.insert(PCM_U24BE, "u24be");
    fnames.insert(PCM_S32LE, "s32le");
    fnames.insert(PCM_S32BE, "s32be");
    fnames.insert(PCM_U32LE, "u32le");
    fnames.insert(PCM_U32BE, "u32be");
    fnames.insert(PCM_FLOAT, "float");
    QString name("Unknown");
    if (fnames.contains(d.data()->format)) {
        name = fnames.value(d.data()->format);
    }

    QStringList list;
    QHash <ChannelPosition, QString> names;
    names.insert(CHAN_NULL, "NA");
    names.insert(CHAN_FRONT_LEFT, "FL");
    names.insert(CHAN_FRONT_RIGHT, "FR");
    names.insert(CHAN_REAR_LEFT, "RL");
    names.insert(CHAN_REAR_RIGHT, "RR");
    names.insert(CHAN_FRONT_CENTER, "FC");
    names.insert(CHAN_REAR_CENTER, "RC");
    names.insert(CHAN_LFE, "LFE");
    names.insert(CHAN_SIDE_LEFT, "SL");
    names.insert(CHAN_SIDE_RIGHT, "SR");
    foreach (const auto  &it, d.data()->channelList) {
        list.append(names.value(it));
    }

    return QString("SampleRate = [%1 Hz], Channels = [%2], AudioFormat = [%3]")
            .arg(d.data ()->sampleRate).arg(list.join(",")).arg (name);
}

void AudioParameters::generateMap(int channels)
{
    int mask = 0;

    switch (channels) {
        case 1:
            mask = CHAN_FRONT_LEFT;
            break;
        case 2:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT;
            break;
        case 3:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT
                    | CHAN_FRONT_CENTER;
            break;
        case 4:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT
                    | CHAN_REAR_LEFT
                    | CHAN_REAR_RIGHT;
            break;
        case 5:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT
                    | CHAN_FRONT_CENTER
                    | CHAN_REAR_LEFT
                    | CHAN_REAR_RIGHT;
            break;
        case 6:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT
                    | CHAN_FRONT_CENTER
                    | CHAN_LFE
                    | CHAN_REAR_LEFT
                    | CHAN_REAR_RIGHT;
            break;
        case 7:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT
                    | CHAN_FRONT_CENTER
                    | CHAN_LFE
                    | CHAN_REAR_CENTER
                    | CHAN_SIDE_LEFT
                    | CHAN_SIDE_RIGHT;
            break;
        case 8:
            mask = CHAN_FRONT_LEFT
                    | CHAN_FRONT_RIGHT
                    | CHAN_FRONT_CENTER
                    | CHAN_LFE
                    | CHAN_REAR_LEFT
                    | CHAN_REAR_RIGHT
                    | CHAN_SIDE_LEFT
                    | CHAN_SIDE_RIGHT;
            break;
        default:
            ;
    }

    for(int i = 0; i < 9; ++i) {
        if(mask & S_INTERNAL_MAP[i])
            d.data()->channelList.append(S_INTERNAL_MAP[i]);
    }
}

} //PhoenixPlayer
