#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPointer>

#include "Common.h"
//#include "Settings.h"
//#include "PlayBackendLoader.h"
//#include "IPlayBackend.h"
//#include "MusicLibraryManager.h"


namespace PhoenixPlayer {
class Settings;

namespace PlayBackend {
class PlayBackendLoader;
class IPlayBackend;
class BaseMediaObject;
}
namespace MusicLibrary {
class MusicLibraryManager;
}
class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);
    virtual ~Player();

    void setPlayBackendLoader(PlayBackend::PlayBackendLoader *loader = 0);
    void setMusicLibraryManager(MusicLibrary::MusicLibraryManager *manager = 0);
    void setSettings(Settings *settings);

    void setPlayMode(Common::PlayMode mode = Common::ModeOrder);
    Q_INVOKABLE void setPlayModeInt (int mode = 0);
    Q_INVOKABLE Common::PlayMode getPlayMode();

protected:
    enum EPointer {
        PNULL = 0x0,
        PPlayBackendLoader,
        PPlaybackend,
        PMusicLibraryManager
    };
signals:
    void playStateChanged(Common::PlaybackState state);
    void playModeChanged(Common::PlayMode mode);
    void playModeChanged (int mode);

    //IPlayBackend的信号
//    void positionChanged(quint64 posMs = 0);
//    void volumeChanged(int vol);
//    //     void message(QMessageBox::Icon icon, const QString &title, const QString &msg) = 0;
//    void mediaChanged(PlayBackend::BaseMediaObject *obj = 0);
//    void finished();
//    void failed();
//    void stateChanged(Common::PlaybackState state);
//    void tick(quint64 msec);
public slots:
    ///
    /// \brief togglePlayPause 切换播放状态, 播放=>暂停,或者暂停=>播放
    ///
    void togglePlayPause();
    void play();
    void stop();
    void pause();
    void setVolume(int vol = 0);
    void setPosition(qreal pos = 0, bool isPercent = true);

private:
    bool PointerValid(EPointer pointer = EPointer::PNULL);
private:
    QPointer<Settings> mSettings;
    QPointer<PlayBackend::PlayBackendLoader> mPlayBackendLoader;
    QPointer<PlayBackend::IPlayBackend> mPlayBackend;
    QPointer<MusicLibrary::MusicLibraryManager> mMusicLibraryManager;
    Common::PlayMode mPlayMode;

};

} //PhoenixPlayer
#endif // PLAYER_H
