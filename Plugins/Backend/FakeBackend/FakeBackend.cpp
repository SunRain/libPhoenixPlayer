#include "FakeBackend.h"
#include "Common.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace FakeBackend {
const static char *BACKEND_NAME = "FakeBackend";
const static char *BACKEND_VERSION = "0.1";
FakeBackend::FakeBackend(QObject *parent) : IPlayBackend(parent)
{
}

PhoenixPlayer::Common::PlayBackendState FakeBackend::getPlayBackendState()
{
    return PhoenixPlayer::Common::PlayBackendStopped;
}

BaseVolume *FakeBackend::baseVolume()
{
    return nullptr;
}

QString FakeBackend::getDescription()
{
    return QString("Fake PlayBackend");
}

QString FakeBackend::getPluginName()
{
    return QString(BACKEND_NAME);
}

QString FakeBackend::getPluginVersion()
{
    return QString(BACKEND_VERSION);
}

void FakeBackend::init()
{
}

void FakeBackend::play(quint64 startMs)
{
    Q_UNUSED(startMs)
}

void FakeBackend::stop()
{
}

void FakeBackend::pause()
{
}

void FakeBackend::setVolume(int vol)
{
    Q_UNUSED(vol)
}

void FakeBackend::setPosition(quint64 posMs)
{
    Q_UNUSED(posMs)
}

void FakeBackend::changeMedia(BaseMediaObject *obj, quint64 startMs, bool startPlay)
{
    Q_UNUSED(obj)
    Q_UNUSED(startMs)
    Q_UNUSED(startPlay)
}

} //FakeBackend
} //PlayBackend
} //PhoenixPlayer
