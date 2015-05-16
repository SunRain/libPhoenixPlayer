#ifndef FAKEBACKEND_H
#define FAKEBACKEND_H

#include "Backend/IPlayBackend.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace FakeBackend {

class Common;
class FakeBackend : public IPlayBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayBackend.FakeBackend" FILE "fakebackend.json")
    Q_INTERFACES(PhoenixPlayer::PlayBackend::IPlayBackend)
public:
    FakeBackend(QObject *parent = 0);

    PhoenixPlayer::Common::PlayBackendState getPlayBackendState();
    QString getDescription();
    QString	getPluginName();
    QString getPluginVersion();
    void	init();

public slots:
    void play(quint64 startMs );
    void stop();
    void pause();
    void setVolume(int vol );
    void setPosition(quint64 posMs );
    void changeMedia(PlayBackend::BaseMediaObject *obj ,quint64 startMs , bool startPlay = false);
};

} //FakeBackend
} //PlayBackend
} //PhoenixPlayer

#endif // FAKEBACKEND_H
