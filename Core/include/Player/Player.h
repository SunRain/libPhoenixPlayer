#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPointer>

#include "Common.h"
//#include "Settings.h"
//#include "PlayBackendLoader.h"
//#include "IPlayBackend.h"
//#include "MusicLibraryManager.h"
#include "MetadataLookup/IMetadataLookup.h"

namespace PhoenixPlayer {
class Settings;
class PluginLoader;

namespace PlayBackend {
//class PlayBackendLoader;
class IPlayBackend;
class BaseMediaObject;
}
namespace MusicLibrary {
class MusicLibraryManager;
}

namespace MetadataLookup {
class MetadataLookupManager;
}

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int playMode READ getPlayModeInt WRITE setPlayMode NOTIFY playModeChanged)
    Q_PROPERTY(int playBackendState READ getPlayBackendStateInt NOTIFY playBackendStateChanged)
public:
    explicit Player(QObject *parent = 0);
    virtual ~Player();

#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    static Player *instance();
#endif

//    void setPlayBackendLoader(PlayBackend::PlayBackendLoader *loader = 0);
//    void setPluginLoader(PluginLoader *loader = 0);
//    void setMusicLibraryManager(MusicLibrary::MusicLibraryManager *manager = 0);
//    void setSettings(Settings *settings);

    void setPlayMode(Common::PlayMode mode = Common::PlayModeOrder);
    void setPlayMode (int mode);
    Common::PlayMode getPlayMode();
    int getPlayModeInt();

    Common::PlayBackendState getPlayBackendState();
    int getPlayBackendStateInt();

    ///
    /// \brief lookupLyric 搜索lyrics歌词
    /// \param songHash 需要搜索的歌曲hash, 空值为当前播放的歌曲
    ///
    Q_INVOKABLE void lookupLyric(const QString &songHash);
    Q_INVOKABLE void lookupAlbumImage(const QString &songHash);
    Q_INVOKABLE void lookupAlbumDescription(const QString &songHash);
    Q_INVOKABLE void lookupAlbumDate(const QString &songHash);
    Q_INVOKABLE void lookupArtistImage(const QString &songHash);
    Q_INVOKABLE void lookupArtistDescription(const QString &songHash);
    Q_INVOKABLE void lookupTrackDescription(const QString &songHash);

protected:
    enum EPointer {
        PNULL = 0x0,
        PPluginLoader,
        PPlaybackend,
        PPLyricsManager,
        PMusicLibraryManager
    };
signals:
    void playBackendStateChanged(Common::PlayBackendState state);
    void playBackendStateChanged (int state);
    void playModeChanged(Common::PlayMode mode);
    void playModeChanged (int mode);

    void lookupLyricSucceed(QString songHash);
    void lookupLyricFailed(QString songHash);

    void lookupAlbumImageSucceed(QString songHash);
    void lookupAlbumImageFailed(QString songHash);

    void lookupAlbumDescriptionSucceed(QString songHash);
    void lookupAlbumDescriptionFailed(QString songHash);

    void lookupAlbumDateSucceed(QString songHash);
    void lookupAlbumDateFailed(QString songHash);

    void lookupArtistImageSucceed(QString songHash);
    void lookupArtistImageFailed(QString songHash);

    void lookupArtistDescriptionSucceed(QString songHash);
    void lookupArtistDescriptionFailed(QString songHash);

    void lookupTrackDescriptionSucceed(QString songHash);
    void lookupTrackDescriptionFailed(QString songHash);

    void metadataLookupFailed(QString songHash);

    ///
    /// \brief playTickActual 播放实际时间
    /// \param sec 实际时间
    ///
    void playTickActual(quint64 sec);
    ///
    /// \brief playTickPercent 播放时间百分比,
    /// \param percent 0~100的播放时间百分比
    ///
    void playTickPercent(int percent);

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
    void init();

    void setPluginLoader();
    void setMusicLibraryManager();
    void setMetaLookupManager();

    bool PointerValid(EPointer pointer = EPointer::PNULL);

    int getSongLength(const QString &hash);

    void doMetadataLookup(const QString &songHash,
                        MetadataLookup::IMetadataLookup::LookupType type);
    void emitMetadataLookupResult(MetadataLookup::IMetadataLookup::LookupType type,
                                  const QString &hash,
                                  bool succeed);

private:
    bool isInit;
    Settings *mSettings;
    PluginLoader *mPluginLoader;
    MusicLibrary::MusicLibraryManager *mMusicLibraryManager;
    MetadataLookup::MetadataLookupManager *mMetaLookupManager;

    QPointer<PlayBackend::IPlayBackend> mPlayBackend;

    Common::PlayMode mPlayMode;
    quint64 mCurrentSongLength;
    quint64 mCurrentPlayPos;

};

} //PhoenixPlayer
#endif // PLAYER_H
