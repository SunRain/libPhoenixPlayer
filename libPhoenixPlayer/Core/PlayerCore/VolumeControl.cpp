#include "PlayerCore/VolumeControl.h"

#include <QDebug>
#include <QTimer>

#include "PluginLoader.h"
#include "PluginHost.h"

#include "Backend/BaseVolume.h"
#include "Backend/IPlayBackend.h"
#include "Backend/SoftVolume.h"
#include "Backend/BackendHost.h"

namespace PhoenixPlayer {

using namespace MusicLibrary;
using namespace PlayBackend;

VolumeControl::VolumeControl(PluginLoader *loader, QObject *parent)
    : QObject(parent)
    , m_pluginLoader(loader)
{
//    m_pluginLoader = phoenixPlayerLib->pluginLoader ();//PluginLoader::instance ();

    m_playBackend = nullptr;
    m_volume = nullptr;

    m_left = 100;
    m_right = 100;
    m_prevLeft = 0;
    m_prevRight = 0;
    m_prev_block = false;
    m_muted = false;

    m_timer = new QTimer;
    m_timer->setSingleShot (false);

//    connect (m_pluginLoader, &PluginLoader::signalPluginChanged,
//             [&](Common::PluginType type) {
//        if (type == Common::PluginPlayBackend) {
////            m_playBackend = m_pluginLoader->getCurrentPlayBackend ();
////            m_volume = m_playBackend->baseVolume ();
//            reload ();
//        }
//    });

//    connect (m_pluginLoader, &PluginLoader::signalPluginChanged, this, &VolumeControl::doPluginChanged);

    connect (m_timer, &QTimer::timeout, this, &VolumeControl::checkVolume);

    reload ();
}

VolumeControl::~VolumeControl()
{
    //TODO should save volume info here or in BaseVolume
    if (m_playBackend)
        m_playBackend = nullptr;
    if (m_volume)
        m_volume = nullptr;
    if (m_timer->isActive ()) {
        m_timer->stop ();
        m_timer->deleteLater ();
        m_timer = nullptr;
    }
}

void VolumeControl::setVolume(int left, int right)
{
    m_muted = false;
    m_volume->setMuted (false);
    m_left = qBound(0, left, 100);
    m_right = qBound(0, right, 100);
    m_volume->setVolume (m_left, m_right);
    checkVolume ();
}

void VolumeControl::setLeftVolume(int left)
{
    setVolume (left, right ());
}

void VolumeControl::setRightVolume(int right)
{
    setVolume (left (), right);
}

void VolumeControl::setVolume(int volume)
{
    volume = qBound(0, volume, 100);
    setVolume (volume - qMax(balance (), 0)*volume/100,
               volume + qMax(balance (), 0)*volume/100);
}

void VolumeControl::setBalance(int balance)
{
    m_muted = false;
    m_volume->setMuted (false);
    balance = qBound(-100, balance, 100);
    setVolume (volume () - qMax(balance, 0)*volume ()/100,
               volume () + qMin(balance, 0)*volume ()/100);
}

int VolumeControl::left() const
{
    return m_left;
}

int VolumeControl::right() const
{
    return m_right;
}

int VolumeControl::volume() const
{
    return qMax(m_left, m_right);
}

int VolumeControl::balance() const
{
    int v = volume ();
    return v > 0 ? (m_right - m_left) * 100/v : 0;
}

void VolumeControl::setMuted(bool muted)
{
    m_muted = muted;
    m_volume->setMuted (muted);
    checkVolume ();
}

bool VolumeControl::muted() const
{
    return m_muted;
}

void VolumeControl::checkVolume()
{
    int left = m_left;
    int right = m_right;
    left = (left > 100) ? 100: left;
    right = (right > 100) ? 100 : right;
    left = (left < 0) ? 0 : left;
    right = (right < 0) ? 0 : right;
    if (m_prevLeft != left || m_prevRight != right) { //volume has been changed
        m_prevLeft = left;
        m_prevRight = right;
//        emit volumeChanged (m_left, m_right);
        emit volumeChanged (volume ());
        emit balanceChanged (balance ());
        emit leftVolumeChanged (m_left);
        emit rightVolumeChanged (m_right);
        emit mutedChanged (m_muted);

    } else if (m_prev_block && !signalsBlocked ()) { //signals have been unblocked
//        emit volumeChanged (m_left, m_right);
        emit volumeChanged (volume ());
        emit balanceChanged (balance ());
        emit leftVolumeChanged (m_left);
        emit rightVolumeChanged (m_right);
        emit mutedChanged (m_muted);
    }
    m_prev_block = signalsBlocked ();
}

void VolumeControl::reload()
{
    qDebug()<<Q_FUNC_INFO<<"================= ";

    m_mutex.lock ();
    if (m_timer->isActive ())
        m_timer->stop ();
    m_mutex.unlock ();

    if (m_volume) {
        m_volume = nullptr;
    }
//    m_playBackend = m_pluginLoader->getCurrentPlayBackend ();
    BackendHost *host = m_pluginLoader->curBackendHost ();
    if (host)
        m_playBackend = host->instance<IPlayBackend>();

    if (m_playBackend) {
        m_volume = m_playBackend->baseVolume ();
    }
    if (m_volume) {
        m_timer->start (150);
    } else {
        m_volume = qobject_cast<BaseVolume *>(SoftVolume::instance ());
        blockSignals (true);
        checkVolume ();
        blockSignals (false);
        QTimer::singleShot (100, this, &VolumeControl::checkVolume);
    }
}

//void VolumeControl::doPluginChanged(Common::PluginType type)
//{
//    if (type == Common::PluginPlayBackend) {
//        reload ();
//    }
//}

} //PhoenixPlayer











