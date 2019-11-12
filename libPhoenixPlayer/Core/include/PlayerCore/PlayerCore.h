#ifndef PLAYERCORE_H
#define PLAYERCORE_H

#include <QObject>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"
#include "AudioMetaObject.h"

namespace PhoenixPlayer {

    class MusicQueue;
    class PlayerCoreInternal;

/*!
 * \brief The PlayerCore class
 * Internal Singleton class
 */
class LIBPHOENIXPLAYER_EXPORT PlayerCore : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int playMode READ playModeInt WRITE setPlayMode NOTIFY playModeChanged)
    Q_PROPERTY(int playBackendState READ playBackendStateInt NOTIFY playBackendStateChangedInt)
    Q_PROPERTY(bool autoSkipForward READ autoSkipForward WRITE setAutoSkipForward NOTIFY autoSkipForwardChanged)
//    Q_PROPERTY(QObject* playList READ playListObject CONSTANT)
    Q_PROPERTY(QVariantMap currentTrack READ currentTrack NOTIFY trackChanged)
    Q_PROPERTY(int forwardIndex READ forwardIndex CONSTANT)
    Q_PROPERTY(int backwardIndex READ backwardIndex CONSTANT)
    Q_PROPERTY(int shuffleIndex READ shuffleIndex CONSTANT)

public:
    explicit PlayerCore(QObject *parent = Q_NULLPTR);

    virtual ~PlayerCore();


    PPCommon::PlayMode playMode() const;
    int playModeInt() const;

    PPCommon::PlayBackendState playBackendState() const;
    int playBackendStateInt() const;

    MusicQueue *playQueue() const;

    AudioMetaObject curTrackMetaObject() const;
    QVariantMap currentTrack() const;

    /*!
     * \brief autoSkipForward 是否在播放结束或者播放失败后自动跳转到下一首歌曲，跳转的歌曲由Common::PlayMode决定
     * \return 是否跳转
     */
    bool autoSkipForward() const;

    /*!
     * \brief playAt play at current playQueue index
     * \param idx
     */
    void playAt(int idx);

    /*!
     * \brief playFromLibrary  play from music library and add to current playQueue
     * \param songHash
     */
    void playFromLibrary(const QString &songHash);

    /*!
     * \brief playFromNetwork play from network and add to current playQueue
     * \param url
     */
    void playFromNetwork(const QUrl &url);

    /*!
     * \brief playTrack without adding to playqueue
     * \param data
     */
    void playTrack(const AudioMetaObject &data);

    /*!
     * \brief forwardIndex playQueue 中当前曲目的下一首index
     * \return -1 如果PlayListMgr中没有曲目，
     * 当使用 PlayMode::PlayModeOrder 如果当前曲目是列表最后一首，返回 -1
     * 当使用 PlayMode::PlayModeRepeatAll 如果当前曲目是列表最后一首，则返回 0
     * 当使用 PlayMode::PlayModeShuffle，返回 -1
     * 当使用 PlayMode::PlayModeRepeatCurrent，返回当前曲目的index
     */
    int forwardIndex() const;

    /*!
     * \brief backwardIndex playQueue 中当前曲目的上一首index
     * \return -1 如果PlayListMgr中没有曲目，
     * 当使用 PlayMode::PlayModeOrder 如果当前曲目是列表第一首，返回 -1
     * 当使用 PlayMode::PlayModeRepeatAll 如果当前曲目是列表第一首，则返回最后一首的index
     * 当使用 PlayMode::PlayModeShuffle，返回 -1
     * 当使用 PlayMode::PlayModeRepeatCurrent，返回当前曲目的index
     */
    int backwardIndex() const;

    /*!
     * \brief shuffleIndex playQueue 中随机曲目index
     * \return -1 playQueue
     */
    int shuffleIndex() const;

signals:
    /*!
     * \brief trackChanged 当切换歌曲的时候发送此信号
     * \param currentTrack
     */
    void trackChanged(const QVariantMap &currentTrack);

    void playTrackFinished();

    void playTrackFailed();

    /*!
     * \brief playTickActual 播放实际时间
     * \param second 实际时间
     */
    void playTickActual(quint64 second);

    /*!
     * \brief playTickPercent 播放时间百分比
     * \param percent 0~100的播放时间百分比
     */
    void playTickPercent(int percent);

    void playModeChanged(PPCommon::PlayMode mode);
    void playModeChangedInt(int mode);

    void playBackendStateChangedInt(int state);

    void playBackendStateChanged(PPCommon::PlayBackendState state);

    void autoSkipForwardChanged(bool skip);

public slots:
    /*!
     * \brief togglePlayPause 切换播放状态, 播放=>暂停,或者暂停=>播放
     */
    void togglePlayPause();

    void play();

    void stop();

    void pause();

    void setPosition(qreal pos = 0, bool isPercent = true);

    /*!
     * \brief skipForward 跳到下一首歌曲
     */
    void skipForward();

    /*!
     * \brief skipBackward 跳到上一首歌曲
     */
    void skipBackward();

    /*!
     * \brief skipShuffle 播放随即歌曲
     */
    void skipShuffle();

    void setPlayMode(PPCommon::PlayMode mode);
    void setPlayModeInt(int mode);

    void setAutoSkipForward(bool autoSkipForward);

private:
    QSharedPointer<PlayerCoreInternal>  m_internal;
};

} //PhoenixPlayer
#endif // PLAYERCORE_H
