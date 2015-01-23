#ifndef FAKEBACKEND_H
#define FAKEBACKEND_H

#include "IPlayBackend.h"

namespace PhoenixPlayer {
class Common;
namespace PlayBackend {
namespace FakeBackend {

class FakeBackend : public IPlayBackend
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayBackend.FakeBackend" FILE "fakebackend.json")
    Q_INTERFACES(PhoenixPlayer::PlayBackend::IPlayBackend)
public:
    FakeBackend(QObject *parent = 0);

    Common::PlaybackState  getPlaybackState() ;
    QString	getBackendName() ;
    QString getBackendVersion() ;
    void	init() ;

public slots:
    void play(quint64 startMs ) ;
    void stop() ;
    void pause() ;
    void setVolume(int vol ) ;
    void setPosition(quint64 posMs ) ;
    void changeMedia(PlayBackend::BaseMediaObject *obj ,quint64 startMs , bool startPlay = false) ;
};

} //FakeBackend
} //PlayBackend
} //PhoenixPlayer

#endif // FAKEBACKEND_H
