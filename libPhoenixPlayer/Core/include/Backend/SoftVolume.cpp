#include "SoftVolume.h"

#include "BaseVolume.h"
#include "Buffer.h"

namespace PhoenixPlayer{
namespace PlayBackend {

SoftVolume::SoftVolume(QObject *parent)
    : BaseVolume(parent)
{
    setVolume (100,100);
    setMuted (false);
}

SoftVolume *SoftVolume::createInstance()
{
    return new SoftVolume;
}

SoftVolume *SoftVolume::instance()
{
     return Singleton<SoftVolume>::instance(SoftVolume::createInstance);
}

SoftVolume::~SoftVolume()
{
}

void SoftVolume::changeVolume(Buffer *buffer, int channels)
{
//    qDebug()<<Q_FUNC_INFO<<QString("change volume, chan = [%1], format = [%2]").arg (chan).arg (format);
//    qDebug()<<QString("leftVolume = [%1], rightVolume = [%2]").arg (leftVolume ()).arg (rightVolume ());
//    qDebug()<<QString("leftScale = [%1], rightScale = [%2]").arg (leftScale ()).arg (rightScale ());

    if(channels == 1) {
        for(size_t i = 0; i < buffer->samples; ++i) {
            buffer->data[i] *= qMax(leftScale(), rightScale());
        }
    } else {
        for(size_t i = 0; i < buffer->samples; i+=2) {
            buffer->data[i] *= leftScale();
            buffer->data[i+1] *= rightScale();
        }
    }
}

} //namespace PlayBackend
} //namespace PhoenixPlayer
