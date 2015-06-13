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
    Q_PROPERTY(bool autoSkipForwad READ getAutoSkipForward WRITE setAutoSkipForward NOTIFY autoSkipForwardChanged)
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
    /// \brief setAutoSkipForward 是否在播放结束或者播放失败后自动跳转到下一首歌曲，跳转的歌曲由Common::PlayMode决定
    /// \param autoSkipForward 是否跳转
    ///
    void setAutoSkipForward(bool autoSkipForward = true);
    bool getAutoSkipForward();

    Q_INVOKABLE void playFromLibrary(const QString &songHash);
    Q_INVOKABLE void playFromNetwork(const QUrl &url);

    //播放队列相关操作
    Q_INVOKABLE void addToQueue(const QString &songHash, bool skipDuplicates = true);
    Q_INVOKABLE QStringList getPlayQueue();
    Q_INVOKABLE bool removeQueueItemAt(int index = -1);
    Q_INVOKABLE void removeAllQueueItem();

    ///
    /// \brief forwardTrackHash 返回下一曲的hash值
    /// \param jumpToFirst 如果是最后一曲，是否最动跳转到第一首曲目
    /// \return
    ///
    Q_INVOKABLE QString forwardTrackHash(bool jumpToFirst = true);
    ///
    /// \brief backwardTrackHash 返回上一曲的hash值
    /// \param jumpToLast 如果是第一首，是否自动跳转到最后一曲
    /// \return
    ///
    Q_INVOKABLE QString backwardTrackHash(bool jumpToLast = true);

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
    /// \brief trackChanged 当切换歌曲的时候发送此信号，慢于MusicLibraryManager的playingSongChanged信号
    ///
    void trackChanged();
    void playTrackFinished();
    void playTrackFailed();
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

    void autoSkipForwardChanged();

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
    ///
    /// \brief skipForward 跳到下一首歌曲
    ///
    void skipForward();
    ///
    /// \brief skipBackward 跳到上一首歌曲
    ///
    void skipBackward();
    ///
    /// \brief skipShuffle 播放随即歌曲
    ///
    void skipShuffle();

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

    void doPlayByPlayMode();

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

    //临时播放器队列
    QStringList mPlayQueue;

    bool mAutoSkipForward;
};

} //PhoenixPlayer
#endif // PLAYER_H
