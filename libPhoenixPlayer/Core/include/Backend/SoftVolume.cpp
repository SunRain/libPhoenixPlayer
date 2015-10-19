#include "SoftVolume.h"

#include "BaseVolume.h"
#include "Buffer.h"

namespace PhoenixPlayer{
class Buffer;
namespace PlayBackend {

SoftVolume::SoftVolume(QObject *parent) :
    BaseVolume(parent)
{
    setVolume (100,100);
    setMuted (false);
}

SoftVolume::~SoftVolume()
{
}

void SoftVolume::changeVolume(Buffer *buffer, int chan, AudioParameters::AudioFormat format)
{
//    qDebug()<<Q_FUNC_INFO<<QString("change volume, chan = [%1], format = [%2]").arg (chan).arg (format);
//    qDebug()<<QString("leftVolume = [%1], rightVolume = [%2]").arg (leftVolume ()).arg (rightVolume ());
//    qDebug()<<QString("leftScale = [%1], rightScale = [%2]").arg (leftScale ()).arg (rightScale ());

    int samples = 0;
    qint32 sample1 = 0;
    qint32 sample2 = 0;
    switch (format) {
    case AudioParameters::PCM_S8: {
        samples = buffer->nbytes;
        if (chan > 1) {
            for (int i=0; i<samples; i+=2) {
                ((char*)buffer->data)[i] *= leftScale ();
                ((char*)buffer->data)[i+1] *= rightScale ();
            }
        } else {
            for (int i=0; i<samples; ++i) {
                ((char*)buffer->data)[i] *= qMax(leftScale (), rightScale ());
            }
        }
        break;
    }
    case AudioParameters::PCM_S16LE: {
        samples = buffer->nbytes/2;
        if (chan > 1) {
            for (int i=0; i<samples; i+=2) {
                ((short*)buffer->data)[i] *= leftScale ();
                ((short*)buffer->data)[i+1] *= rightScale ();
            }
        } else {
            for (int i = 0; i < samples; ++i) {
                ((short*)buffer->data)[i] *= qMax(leftScale (), rightScale ());
            }
        }
        break;
    }
    case AudioParameters::PCM_S24LE: {
        samples = buffer->nbytes/4;
        if (chan > 1) {
            for (qint64 i=0; i<samples; i+=2) {
                sample1 = ((qint32*)buffer->data)[i];
                sample2 = ((qint32*)buffer->data)[i+1];

                if (sample1 & 0x800000)
                    sample1 |= 0xff000000;

                if (sample2 & 0x800000)
                    sample2 |= 0xff000000;

                sample1 *= leftScale ();//m_scaleLeft;
                sample2 *= rightScale ();//m_scaleRight;

                ((qint32*)buffer->data)[i] = sample1;
                ((qint32*)buffer->data)[i+1] = sample2;
            }
        } else {
            for (qint64 i=0; i<samples; ++i) {
                sample1 = ((qint32*)buffer->data)[i];
                sample1 *= qMax(leftScale (), rightScale ());

                if (sample1 & 0x800000)
                    sample1 |= 0xff000000;

                ((qint32*)buffer->data)[i] = sample1;
            }
        }
        break;
    }
    case AudioParameters::PCM_S32LE: {
        samples = buffer->nbytes/4;
        if (chan > 1) {
            for (qint64 i = 0; i < samples; i+=2) {
                ((qint32*)buffer->data)[i] *= leftScale ();
                ((qint32*)buffer->data)[i+1] *= rightScale ();
            }
        } else {
            for (qint64 i=0; i<samples; ++i) {
                ((qint32*)buffer->data)[i]*= qMax(leftScale (), rightScale ());
            }
        }
        break;
    }
    default:
        break;
    }
}

} //namespace PlayBackend
} //namespace PhoenixPlayer
