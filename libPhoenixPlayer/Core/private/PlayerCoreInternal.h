#ifndef PLAYERCOREINTERNAL_H
#define PLAYERCOREINTERNAL_H

#include <QObject>
#include <QSharedPointer>

#include "AudioMetaObject.h"
#include "PPCommon.h"
#include "PluginMgr.h"

namespace PhoenixPlayer {

    class PluginMgrInternal;
    class MusicLibraryManagerInternal;
    class RecentPlayedQueue;
    class MusicQueue;
    class MediaResource;

    namespace PlayBackend {
        class IPlayBackend;
    }

class PlayerCoreInternal : public QObject
{
    Q_OBJECT
public:
    explicit PlayerCoreInternal(QSharedPointer<PluginMgrInternal> mgr,
                                QSharedPointer<MusicLibraryManagerInternal> library,
                                QObject *parent = Q_NULLPTR);
    virtual ~PlayerCoreInternal();

    MusicQueue *playQueue() const
    {
        return m_playQueue;
    }

    void playAt(int idx);
    void playTrack(const AudioMetaObject &data);
    void playFromLibrary(const QString &songHash);
    void playFromNetwork(const QUrl &url);

    void skipForward();
    void skipBackward();
    void skipShuffle();

    int forwardIndex() const;
    int backwardIndex() const;
    int shuffleIndex() const;

    inline bool autoSkipForward() const
    {
        return m_autoSkipForward;
    }
    void setAutoSkipForward(bool autoSkipForward);

    void setPlayMode(PPCommon::PlayMode mode);

    inline PPCommon::PlayMode playMode() const
    {
        return m_playMode;
    }

    PPCommon::PlayBackendState playBackendState() const;

    inline AudioMetaObject curTrackMetaObject() const
    {
        return m_curTrack;
    }

    void togglePlayPause();
    void play();
    void stop();
    void pause();
    void setPosition(qreal pos = 0, bool isPercent = true);

signals:
    void trackChanged(const QVariantMap &currentTrack);

    void playTrackFinished();

    void playTrackFailed();

    void playTickActual(quint64 second);

    void playTickPercent(int percent);

    void playModeChanged(PPCommon::PlayMode mode);

    void playModeChangedInt(int mode);

    void playBackendStateChangedInt(int state);

    void playBackendStateChanged(PPCommon::PlayBackendState state);

    void autoSkipForwardChanged(bool skip);

private:
    void initPlayBackend();

    void doPlayByPlayMode();

private:
    PlayBackend::IPlayBackend                   *m_playBackend      = Q_NULLPTR;
    MusicQueue                                  *m_playQueue        = Q_NULLPTR;
    MediaResource                               *m_resource         = Q_NULLPTR;

    QSharedPointer<PluginMgrInternal>           m_pluginMgr;
    QSharedPointer<MusicLibraryManagerInternal> m_library;

    bool                                        m_autoSkipForward   = false;
    quint64                                     m_curTrackDuration  = 0;
    quint64                                     m_currentPlayPos    = 0;
    PPCommon::PlayMode                          m_playMode          = PPCommon::PlayModeOrder;

    PluginMetaData                              m_usedPlayBackend;
    AudioMetaObject                             m_curTrack;
};

} //namespace PhoenixPlayer
#endif // PLAYERCOREINTERNAL_H
