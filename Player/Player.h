#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPointer>

//#include "Common.h"
//#include "Settings.h"
//#include "PlayBackendLoader.h"
//#include "IPlayBackend.h"
//#include "MusicLibraryManager.h"


namespace PhoenixPlayer {

class Settings;
namespace PlayBackend {
class PlayBackendLoader;
class IPlayBackend;
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

signals:

public slots:


private:
    QPointer<Settings> mSettings;
    QPointer<PlayBackend::PlayBackendLoader> mPlayBackendLoader;
    QPointer<PlayBackend::IPlayBackend> mPlayBackend;
    QPointer<MusicLibrary::MusicLibraryManager> mLibraryManager;

};

} //PhoenixPlayer
#endif // PLAYER_H
