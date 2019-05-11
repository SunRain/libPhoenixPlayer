#ifndef PLAYERCORE_H
#define PLAYERCORE_H

#include <QObject>
#include <QPointer>

#include "libphoenixplayer_global.h"
#include "Common.h"
#include "MetadataLookup/IMetadataLookup.h"
#include "SingletonPointer.h"
#include "AudioMetaObject.h"

namespace PhoenixPlayer {
class PPSettings;
class PluginLoader;
class PlayListObject;
class PlayListObjectMgr;
class MediaResource;
//class AudioMetaObject;
//class MusicQueue;
class RecentPlayedMgr;
class MusicQueue;

namespace PlayBackend {
class IPlayBackend;
class BaseMediaObject;
class BackendHost;
}

namespace MusicLibrary {
class MusicLibraryManager;
class IMusicLibraryDAO;
}

namespace MetadataLookup {
class MetadataLookupMgr;
}

class LIBPHOENIXPLAYER_EXPORT PlayerCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int playMode READ playModeInt WRITE setPlayMode NOTIFY playModeChanged)
    Q_PROPERTY(int playBackendState READ playBackendStateInt NOTIFY playBackendStateChanged)
    Q_PROPERTY(bool autoSkipForward READ autoSkipForward WRITE setAutoSkipForward NOTIFY autoSkipForwardChanged)
//    Q_PROPERTY(QObject* playList READ playListObject CONSTANT)
    Q_PROPERTY(QVariantMap currentTrack READ currentTrack NOTIFY trackChanged)
    Q_PROPERTY(int forwardIndex READ forwardIndex CONSTANT)
    Q_PROPERTY(int backwardIndex READ backwardIndex CONSTANT)
    Q_PROPERTY(int shuffleIndex READ shuffleIndex CONSTANT)

    friend class LibPhoenixPlayer;
protected:
    explicit PlayerCore(PPSettings *set, PluginLoader *loader, MusicLibrary::MusicLibraryManager *mgr, QObject *parent = 0);

public:
    virtual ~PlayerCore();
    void initiate();

    Common::PlayMode playMode() const;
    int playModeInt() const;

    Common::PlayBackendState playBackendState() const;
    int playBackendStateInt() const;

//    PlayListObject *playList() const;
    RecentPlayedMgr *recentList() const;
    MusicQueue *playQueue() const;
//    QObject *playListObject() const;

    AudioMetaObject curTrackMetadata();
    QVariantMap currentTrack() const;

    ///
    /// \brief setAutoSkipForward 是否在播放结束或者播放失败后自动跳转到下一首歌曲，跳转的歌曲由Common::PlayMode决定
    /// \param autoSkipForward 是否跳转
    ///
    bool autoSkipForward() const;

    ///
    /// \brief playAt play at current current playQueue index
    /// \param idx
    ///
    Q_INVOKABLE void playAt(int idx);

    ///
    /// \brief playFromLibrary play from music library and add to current playQueue
    /// \param songHash
    ///
    Q_INVOKABLE void playFromLibrary(const QString &songHash);

    ///
    /// \brief playFromNetwork play from network and add to current playQueue
    /// \param url
    ///
    Q_INVOKABLE void playFromNetwork(const QUrl &url);
    void playTrack(const AudioMetaObject &data);

    //播放队列相关操作
//    Q_INVOKABLE void addToQueue(const QString &songHash, bool skipDuplicates = true);
//    Q_INVOKABLE QStringList getPlayQueue();
//    Q_INVOKABLE bool removeQueueItemAt(int index = -1);
//    Q_INVOKABLE void removeAllQueueItem();

    ///
    /// \brief forwardIndex PlayListMgr 中但前曲目的下一首index
    /// \return -1 如果PlayListMgr中没有曲目，
    /// 当使用 PlayMode::PlayModeOrder 如果当前曲目是列表最后一首，返回 -1
    /// 当使用 PlayMode::PlayModeRepeatAll 如果当前曲目是列表最后一首，则返回 0
    /// 当使用 PlayMode::PlayModeShuffle，返回 -1
    /// 当使用 PlayMode::PlayModeRepeatCurrent，返回当前曲目的index
    ///
    int forwardIndex() const;

    ///
    /// \brief backwardIndex PlayListMgr 中但前曲目的上一首index
    /// \return -1 如果PlayListMgr中没有曲目，
    /// 当使用 PlayMode::PlayModeOrder 如果当前曲目是列表第一首，返回 -1
    /// 当使用 PlayMode::PlayModeRepeatAll 如果当前曲目是列表第一首，则返回最后一首的index
    /// 当使用 PlayMode::PlayModeShuffle，返回 -1
    /// 当使用 PlayMode::PlayModeRepeatCurrent，返回当前曲目的index
    ///
    int backwardIndex() const;

    ///
    /// \brief shuffleIndex PlayListMgr 中随机曲目index
    /// \return -1 如果PlayListMgr中没有曲目
    ///
    int shuffleIndex() const;

protected:
//    explicit Player(QObject *parent = 0);
//    enum EPointer {
//        PNULL = 0x0,
//        PPluginLoader,
//        PPlaybackend,
////        PPLyricsManager,
//        PMusicLibraryManager
//    };
signals:
//    void playBackendStateChanged(Common::PlayBackendState state);
//    void playBackendStateChanged (int state);
//    void playModeChanged(Common::PlayMode mode);
//    void playModeChanged (int mode);

    ///
    /// \brief trackChanged 当切换歌曲的时候发送此信号
    ///
    void trackChanged(QVariantMap currentTrack);
    void playTrackFinished();
    void playTrackFailed();
    ///
    /// \brief playTickActual 播放实际时间
    /// \param second 实际时间
    ///
    void playTickActual(quint64 second);
    ///
    /// \brief playTickPercent 播放时间百分比,
    /// \param percent 0~100的播放时间百分比
    ///
    void playTickPercent(int percent);

//    void autoSkipForwardChanged();

    void playModeChanged(Common::PlayMode mode);
    void playModeChanged(int mode);

    void playBackendStateChanged(int state);

    void autoSkipForwardChanged(bool skip);

public slots:
    ///
    /// \brief togglePlayPause 切换播放状态, 播放=>暂停,或者暂停=>播放
    ///
    void togglePlayPause();
    void play();
    void stop();
    void pause();
//    void setVolume(int vol = 0);
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

    void setPlayMode(Common::PlayMode mode);
    void setPlayMode(int mode);

    void setAutoSkipForward(bool autoSkipForward);

private:
    void setPluginLoader();
//    void setMusicLibraryManager();
//    void setMetaLookupManager();

//    bool PointerValid(EPointer pointer = EPointer::PNULL);

//    int getSongLength(const QString &hash);

//    void doMetadataLookup(const QString &songHash,
//                        MetadataLookup::IMetadataLookup::LookupType type);
//    void emitMetadataLookupResult(MetadataLookup::IMetadataLookup::LookupType type,
//                                  const QString &hash,
//                                  bool succeed);

    void doPlayByPlayMode();

private:
//    bool m_isInit;
    PPSettings                                *m_settings;
    PluginLoader *m_pluginLoader;
    MusicLibrary::MusicLibraryManager       *m_musicLibraryManager;
    PlayBackend::IPlayBackend               **m_playBackend;
    PlayBackend::IPlayBackend               *m_pb;
    PlayBackend::BackendHost                *m_playBackendHost;
    AudioMetaObject                         m_curTrack;
//    PlayListObject                          *m_playlistObject;
//    PlayListObjectMgr                       *m_plstObjMgr;
    MusicQueue                              *m_playQueue;
    RecentPlayedMgr                         *m_recentList;
    MusicLibrary::IMusicLibraryDAO          *m_dao;
    MediaResource                           *m_resource;

    Common::PlayMode                        m_playMode;
    quint64                                 m_curTrackDuration;
    quint64                                 m_currentPlayPos;

    //临时播放器队列
//    QStringList m_playQueue;
    bool                                    m_autoSkipForward;
    int                                     m_shuffleIndex;
};

} //PhoenixPlayer
#endif // PLAYERCORE_H
