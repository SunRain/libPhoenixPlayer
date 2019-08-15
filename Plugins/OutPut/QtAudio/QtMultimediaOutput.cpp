#include "QtMultimediaOutput.h"

#include <QMetaObject>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QSettings>
#include <QDebug>
#include <unistd.h>

#include "AudioParameters.h"

namespace PhoenixPlayer {
namespace OutPut {
namespace QtMultimediaOutput {

QtMultimediaOutput::QtMultimediaOutput(QObject *parent)
    : IOutPut (parent),
    m_buffer(Q_NULLPTR),
    m_bytes_per_second(0)
{

}

QtMultimediaOutput::~QtMultimediaOutput()
{

}

bool QtMultimediaOutput::initialize(quint32 sampleRate, const QList<AudioParameters::ChannelPosition> &channels, AudioParameters::AudioFormat f)
{
    IOutPut::initialize(sampleRate, channels, f);

    QAudioFormat qformat;
    qformat.setCodec("audio/pcm");
    qformat.setSampleRate(sampleRate);
    qformat.setByteOrder(QAudioFormat::LittleEndian);
    qformat.setChannelCount(channels.size());
    qformat.setSampleType(QAudioFormat::SignedInt);

    //Size of sample representation in input data. For 24-bit is 4, high byte is ignored.
    qint64 bytes_per_sample = AudioParameters::sampleSize(f);

    switch (f)
    {
    case AudioParameters::PCM_S8:
        qformat.setSampleSize(8);
        break;
    case AudioParameters::PCM_S16LE:
        qformat.setSampleSize(16);
        break;
    case AudioParameters::PCM_S24LE:
        qformat.setSampleSize(24);
        break;
    case AudioParameters::PCM_S32LE:
        qformat.setSampleSize(32);
        break;
    default:
        break;
    }

    if (!qformat.isValid())
        return false;

//    const QSettings settings(Qmmp::configFile(), QSettings::IniFormat);
//    const QString saved_device_name = settings.value("QTMULTIMEDIA/device").toString();

    m_bytes_per_second = bytes_per_sample * sampleRate * qformat.channelCount();

    QAudioDeviceInfo device_info;
//    if (!saved_device_name.isEmpty())
//    {
//        const QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
//        foreach (const QAudioDeviceInfo &info, devices)
//        {
//            if (info.deviceName()==saved_device_name)
//            {
//                if (info.isFormatSupported(qformat))
//                {
//                    device_info = info;
//                    break;
//                }
//                else
//                    qDebug() << "OutputQtMultimedia: Output device: " << saved_device_name << " is not supported";
//            }
//        }
//    }

    if (device_info.isNull())
    {
        device_info = QAudioDeviceInfo::defaultOutputDevice();
        if (!device_info.isFormatSupported(qformat)) {
            qDebug()<<Q_FUNC_INFO<<"Can't find device for output";
            return false;
        }
    }

    qDebug()<<Q_FUNC_INFO<<"Using output device: " << device_info.deviceName();

    m_output.reset(new QAudioOutput(device_info, qformat));
    m_buffer = m_output->start();
    m_control.reset(new OutputControl(m_output.data()));

    return true;
}

qint64 QtMultimediaOutput::latency()
{
    return 0;
}

qint64 QtMultimediaOutput::writeAudio(unsigned char *data, qint64 maxSize)
{
    if (!m_output->bytesFree()) {
        //If the buffer is full, waiting for some bytes to be played:
        //trying to play maxSize bytes, but not more than half of buffer.
        usleep(qMin(maxSize, static_cast<qint64>(m_output->bufferSize() / 2)) * 1000000 / m_bytes_per_second);
    }
    return m_buffer->write((const char*)data, maxSize);
}

void QtMultimediaOutput::drain()
{
    m_buffer->waitForBytesWritten(-1);
}

void QtMultimediaOutput::reset()
{
    m_buffer->reset();
}

void QtMultimediaOutput::suspend()
{
    QMetaObject::invokeMethod(m_control.data(), "suspend", Qt::QueuedConnection);
}

void QtMultimediaOutput::resume()
{
    QMetaObject::invokeMethod(m_control.data(), "resume", Qt::QueuedConnection);
}

OutputControl::OutputControl(QAudioOutput *o)
    : m_output(o)
{

}

void OutputControl::suspend()
{
    m_output->suspend();
}

void OutputControl::resume()
{
    m_output->resume();
}







} //QtMultimediaOutput
} //OutPut
} //PhoenixPlayer
