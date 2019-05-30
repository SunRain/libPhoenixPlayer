#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <PPCommon.h>
#include <QObject>

#include "libphoenixplayer_global.h"
#include "SingletonPointer.h"

class QTimer;
namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;

namespace PlayBackend {
class IPlayBackend;
class BaseVolume;
}
class LIBPHOENIXPLAYER_EXPORT VolumeControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int leftVolume READ left WRITE setLeftVolume NOTIFY leftVolumeChanged)
    Q_PROPERTY(int rightVolume READ right WRITE setRightVolume NOTIFY rightVolumeChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int balance READ balance WRITE setBalance NOTIFY balanceChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    friend class LibPhoenixPlayer;
protected:
    explicit VolumeControl(PluginLoader *loader, QObject *parent = Q_NULLPTR);
public:
    virtual ~VolumeControl();

    void setVolume(int left, int right);

    void setLeftVolume(int left);
    int left() const;

    void setRightVolume(int right);
    int right() const;

    void setVolume(int volume);
    ///
    /// \brief Returns the maximum volume of the left and right channels.
    /// \return
    ///
    int volume() const;

    ///
    /// \brief Sets the balance between left and right channels.
    /// \param balance balance between left and right channels [-100..100].
    ///
    void setBalance(int balance);

    ///
    /// \brief Returns the balance between left and right channels.
    /// \return
    ///
    int balance() const;

    void setMuted(bool muted = false);
    bool muted() const;

signals:
//    void volumeChanged(int left, int right);
    void volumeChanged(int volume);
    void balanceChanged(int balance);
    void leftVolumeChanged(int volume);
    void rightVolumeChanged(int volume);
    void mutedChanged(bool muted);
public slots:
    ///
    /// \brief Forces the volumeChanged signal to emit.
    ///
    void checkVolume();

    ///
    /// \brief Updates volume configuration
    ///
    void reload();
private:
//    void doPluginChanged(Common::PluginType type);
    void changeMuted(bool muted);

private:
    PluginLoader *m_pluginLoader;
    PlayBackend::IPlayBackend *m_playBackend;
    PlayBackend::BaseVolume *m_volume;
    int m_left;
    int m_prevLeft;
    int m_leftBeforeMuted;
    int m_right;
    int m_prevRight;
    int m_rightBeforMuted;
    bool m_prev_block;
    bool m_muted;
    QTimer *m_timer;
    QMutex m_mutex;
};

} //PhoenixPlayer
#endif // VOLUMECONTROL_H
