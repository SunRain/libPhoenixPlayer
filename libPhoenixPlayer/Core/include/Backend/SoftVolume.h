#ifndef SOFTVOLUME_H
#define SOFTVOLUME_H

#include "libphoenixplayer_global.h"
#include "BaseVolume.h"

#include "AudioParameters.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer{
class Buffer;

namespace PlayBackend {
class BaseVolume;

class LIBPHOENIXPLAYER_EXPORT SoftVolume : public BaseVolume
{
    Q_OBJECT
private:
    explicit SoftVolume(QObject *parent = Q_NULLPTR);
    static SoftVolume *createInstance();

public:
    static SoftVolume *instance();

    virtual ~SoftVolume();
    void changeVolume(Buffer *buffer, int chan, AudioParameters::AudioFormat format);
};

} //namespace PlayBackend
} //namespace PhoenixPlayer

#endif // SOFTVOLUME_H
