#ifndef SOFTVOLUME_H
#define SOFTVOLUME_H

#include "BaseVolume.h"

#include "AudioParameters.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer{
class Buffer;

namespace PlayBackend {
class BaseVolume;

class SoftVolume : public BaseVolume
{
    Q_OBJECT
    DECLARE_SINGLETON_POINTER(SoftVolume)
public:
//    explicit SoftVolume(QObject *parent = 0);
    virtual ~SoftVolume();
    void changeVolume(Buffer *buffer, int chan, AudioParameters::AudioFormat format);
};

} //namespace PlayBackend
} //namespace PhoenixPlayer

#endif // SOFTVOLUME_H
