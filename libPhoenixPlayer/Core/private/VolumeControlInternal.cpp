#include "VolumeControlInternal.h"

#include <QDebug>

#include "PluginMgrInternal.h"
#include "Backend/BaseVolume.h"
#include "Backend/IPlayBackend.h"

namespace PhoenixPlayer {

VolumeControlInternal::VolumeControlInternal(QSharedPointer<PluginMgrInternal> mgr, QObject *parent)
    : QObject(parent),
    m_pluginMgr(mgr)
{
    m_left = 100;
    m_right = 100;
    m_prevLeft = 0;
    m_prevRight = 0;
    m_leftBeforeMuted = 0;
    m_rightBeforMuted = 0;
    m_prev_block = false;
    m_muted = false;

    m_timer = new QTimer;
    m_timer->setSingleShot(false);

    m_volume = Q_NULLPTR;

    connect (m_timer, &QTimer::timeout, this, &VolumeControlInternal::checkVolume);

    reload();
}

VolumeControlInternal::~VolumeControlInternal()
{
    if (m_timer->isActive()) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = Q_NULLPTR;
    }

    if (PluginMetaData::isValid(m_usedPlayBackend)) {
        PluginMgr::unload(m_usedPlayBackend);
    }
}

void VolumeControlInternal::setVolume(int left, int right)
{
    m_muted = false;
    m_left = qBound(0, left, 100);
    m_right = qBound(0, right, 100);
    if (m_volume) {
        m_volume->setMuted(false);
        m_volume->setVolume(m_left, m_right);
    }
    checkVolume();
}

void VolumeControlInternal::setLeftVolume(int left)
{
    setVolume(left, right());
}

void VolumeControlInternal::setRightVolume(int right)
{
    setVolume(left(), right);
}

void VolumeControlInternal::setVolume(int volume)
{
    volume = qBound(0, volume, 100);
    setVolume(volume - qMax(balance(), 0)*volume/100,
              volume + qMax(balance(), 0)*volume/100);
}

void VolumeControlInternal::setBalance(int balance)
{
    m_muted = false;
    if (m_volume) {
        m_volume->setMuted(false);
    }
    balance = qBound(-100, balance, 100);
    setVolume (volume() - qMax(balance, 0)*volume ()/100,
              volume() + qMin(balance, 0)*volume ()/100);
}

void VolumeControlInternal::setMuted(bool muted)
{
    m_muted = muted;
    if (m_volume) {
        m_volume->setMuted (muted);
    }
    changeMuted(muted);
}

void VolumeControlInternal::checkVolume()
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
        emit volumeChanged(volume());
        emit balanceChanged(balance());
        emit leftVolumeChanged(m_left);
        emit rightVolumeChanged(m_right);
        //        emit mutedChanged (m_muted);

    } else if (m_prev_block && !signalsBlocked()) { //signals have been unblocked
        //        emit volumeChanged (m_left, m_right);
        emit volumeChanged(volume());
        emit balanceChanged(balance());
        emit leftVolumeChanged(m_left);
        emit rightVolumeChanged(m_right);
        //        emit mutedChanged (m_muted);
    }
    m_prev_block = signalsBlocked();
}

void VolumeControlInternal::reload()
{
    qDebug()<<Q_FUNC_INFO<<"================= ";

    m_mutex.lock();
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_mutex.unlock();

    if (PluginMetaData::isValid(m_usedPlayBackend)) {
        PluginMgr::unload(m_usedPlayBackend);
    }
    if (m_volume) {
        m_volume = Q_NULLPTR;
    }
    m_usedPlayBackend = m_pluginMgr->usedPlayBackend();
    if (PluginMetaData::isValid(m_usedPlayBackend)) {
        PlayBackend::IPlayBackend *bk = qobject_cast<PlayBackend::IPlayBackend*>(PluginMgr::instance(m_usedPlayBackend));
        if (!bk) {
            PluginMgr::unload(m_usedPlayBackend);
            return;
        }
        m_volume = bk->baseVolume();
        if (m_volume) {
            m_timer->start(150);
        } else {
            //        m_volume = qobject_cast<BaseVolume *>(SoftVolume::instance ());
            //        blockSignals (true);
            //        checkVolume ();
            //        blockSignals (false);
            //        QTimer::singleShot (100, this, &VolumeControl::checkVolume);
            qWarning()<<Q_FUNC_INFO<<"No Volume found!!!!";
        }
    } else {
        qWarning()<<Q_FUNC_INFO<<"No valid playbackend found!!!!";
    }
    qDebug()<<Q_FUNC_INFO<<" ================= finish";
}

void VolumeControlInternal::changeMuted(bool muted)
{
    if (muted) {
        m_leftBeforeMuted = m_left;
        m_rightBeforMuted = m_right;
        m_left = 0;
        m_right = 0;
    } else {
        m_left = m_leftBeforeMuted;
        m_right = m_rightBeforMuted;
    }
    checkVolume();
    emit mutedChanged(m_muted);
}








} //namespace PhoenixPlayer
