#include "PlayerCore/VolumeControl.h"

#include <QDebug>
#include <QTimer>

//#include "PluginLoader.h"
//#include "PluginHost.h"

//#include "Backend/BaseVolume.h"
//#include "Backend/IPlayBackend.h"
//#include "Backend/BackendHost.h"

#include "private/VolumeControlInternal.h"
#include "private/SingletonObjectFactory.h"

namespace PhoenixPlayer {

//using namespace MusicLibrary;
using namespace PlayBackend;

VolumeControl::VolumeControl(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->volumeControlInternal();

    connect(m_internal.data(), &VolumeControlInternal::volumeChanged,
            this, &VolumeControl::volumeChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &VolumeControlInternal::balanceChanged,
            this, &VolumeControl::balanceChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &VolumeControlInternal::leftVolumeChanged,
            this, &VolumeControl::leftVolumeChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &VolumeControlInternal::rightVolumeChanged,
            this, &VolumeControl::rightVolumeChanged, Qt::QueuedConnection);

    connect(m_internal.data(), &VolumeControlInternal::mutedChanged,
            this, &VolumeControl::mutedChanged, Qt::QueuedConnection);
}

VolumeControl::~VolumeControl()
{
    m_internal->disconnect(this);
}

void VolumeControl::setVolume(int left, int right)
{
    m_internal->setVolume(left, right);
}

void VolumeControl::setLeftVolume(int left)
{
    m_internal->setLeftVolume(left);
}

void VolumeControl::setRightVolume(int right)
{
    m_internal->setRightVolume(right);
}

void VolumeControl::setVolume(int volume)
{
    m_internal->setVolume(volume);
}

void VolumeControl::setBalance(int balance)
{
    m_internal->setBalance(balance);
}

int VolumeControl::left() const
{
    return m_internal->left();
}

int VolumeControl::right() const
{
    return m_internal->right();
}

int VolumeControl::volume() const
{
    return m_internal->volume();
}

int VolumeControl::balance() const
{
    return m_internal->balance();
}

void VolumeControl::setMuted(bool muted)
{
    m_internal->setMuted(muted);
}

bool VolumeControl::muted() const
{
    return m_internal->muted();
}

void VolumeControl::checkVolume()
{
    m_internal->checkVolume();
}

void VolumeControl::reload()
{
    m_internal->reload();
}


} //PhoenixPlayer











