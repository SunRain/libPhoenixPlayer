#ifndef SOFTVOLUME_H
#define SOFTVOLUME_H

#include "libphoenixplayer_global.h"
#include "Backend/BaseVolume.h"

//#include "AudioParameters.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer{
    namespace PlayBackend {
        namespace PhoenixBackend {

        class Buffer;

class LIBPHOENIXPLAYER_EXPORT SoftVolume : public BaseVolume
{
    Q_OBJECT
private:
    explicit SoftVolume(QObject *parent = Q_NULLPTR);
    static SoftVolume *createInstance();

public:
    static SoftVolume *instance();

    virtual ~SoftVolume();

    void changeVolume(Buffer *buffer, int channels);
};

} //PhoenixPlayer
} //PlayBackend
} //PhoenixPlayer

#endif // SOFTVOLUME_H
