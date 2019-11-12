#ifndef VOLUMECONTROLINTERNAL_H
#define VOLUMECONTROLINTERNAL_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QMutex>

#include "PluginMgr.h"

namespace PhoenixPlayer {

    class PluginMgrInternal;

    namespace PlayBackend {
        class BaseVolume;
    }

/*!
 * \brief The VolumeControlInternal class
 * Internal Singleton class
 */
class VolumeControlInternal : public QObject
{
    Q_OBJECT
public:
    explicit VolumeControlInternal(QSharedPointer<PluginMgrInternal> mgr, QObject *parent = Q_NULLPTR);
    virtual ~VolumeControlInternal();

    void setVolume(int left, int right);

    void setLeftVolume(int left);
    inline int left() const
    {
        return m_left;
    }

    void setRightVolume(int right);
    inline int right() const
    {
        return m_right;
    }

    void setVolume(int volume);

    inline int volume() const
    {
        return qMax(m_left, m_right);
    }

    void setBalance(int balance);

    inline int balance() const
    {
        int v = volume();
        return v > 0 ? (m_right - m_left) * 100/v : 0;
    }

    void setMuted(bool muted = false);
    inline bool muted() const
    {
        return m_muted;
    }

signals:
    void volumeChanged(int volume);
    void balanceChanged(int balance);
    void leftVolumeChanged(int volume);
    void rightVolumeChanged(int volume);
    void mutedChanged(bool muted);

public slots:
    void checkVolume();

    void reload();

private:
    void changeMuted(bool muted);

private:
    int     m_left;
    int     m_prevLeft;
    int     m_leftBeforeMuted;
    int     m_right;
    int     m_prevRight;
    int     m_rightBeforMuted;
    bool    m_prev_block;
    bool    m_muted;
    QTimer  *m_timer;
    QMutex  m_mutex;

    PluginMetaData m_usedPlayBackend;

    PlayBackend::BaseVolume *m_volume;

    QSharedPointer<PluginMgrInternal> m_pluginMgr;


};

} //namespace PhoenixPlayer
#endif // VOLUMECONTROLINTERNAL_H
