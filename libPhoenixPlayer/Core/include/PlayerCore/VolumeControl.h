#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QObject>
#include <QSharedPointer>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class VolumeControlInternal;

/*!
 * \brief The VolumeControl class
 * Internal Singleton class
 */
class LIBPHOENIXPLAYER_EXPORT VolumeControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int leftVolume READ left WRITE setLeftVolume NOTIFY leftVolumeChanged)
    Q_PROPERTY(int rightVolume READ right WRITE setRightVolume NOTIFY rightVolumeChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int balance READ balance WRITE setBalance NOTIFY balanceChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)

public:
    explicit VolumeControl(QObject *parent = Q_NULLPTR);

    virtual ~VolumeControl();

    void setVolume(int left, int right);

    void setLeftVolume(int left);
    int left() const;

    void setRightVolume(int right);
    int right() const;

    void setVolume(int volume);

    /*!
     * \brief volume
     * \return the maximum volume of the left and right channels.
     */
    int volume() const;

    /*!
     * \brief setBalance Sets the balance between left and right channels.
     * \param balance balance between left and right channels [-100..100].
     */
    void setBalance(int balance);

    /*!
     * \brief balance
     * \return the balance between left and right channels.
     */
    int balance() const;

    void setMuted(bool muted = false);
    bool muted() const;

signals:
    void volumeChanged(int volume);
    void balanceChanged(int balance);
    void leftVolumeChanged(int volume);
    void rightVolumeChanged(int volume);
    void mutedChanged(bool muted);

public slots:
    /*!
     * \brief checkVolume
     * Forces the volumeChanged signal to emit.
     */
    void checkVolume();

    /*!
     * \brief reload
     * Updates volume configuration
     */
    void reload();
private:
    QSharedPointer<VolumeControlInternal> m_internal;
};

} //PhoenixPlayer
#endif // VOLUMECONTROL_H
