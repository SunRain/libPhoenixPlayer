#include "BaseVolume.h"

#include "Buffer.h"
namespace PhoenixPlayer{
namespace PlayBackend {

BaseVolume::BaseVolume(QObject *parent) : QObject(parent)
{
    //TODO: add settings
    m_left = 100;
    m_right = m_left;
    m_scaleLeft = (qreal)m_left/100.0;
    m_scaleRight = m_scaleLeft;
    m_muted = false;
}

BaseVolume::~BaseVolume()
{
    //TODO save to settings
}

void BaseVolume::setVolume(int left, int right)
{
    m_left = left;
    m_right = right;
    m_scaleLeft = (qreal)m_left/100.0;
    m_scaleRight = (qreal)m_right/100.0;
}

void BaseVolume::setVolume(int volume)
{
    this->setVolume (volume, volume);
}

int BaseVolume::leftVolume() const
{
    return m_left;
}

int BaseVolume::rightVolume() const
{
    return m_right;
}

int BaseVolume::volume() const
{
    //FIXME better method to handle this?
    return qMax(m_left, m_right);
}

void BaseVolume::setMuted(bool muted)
{
    m_muted = muted;
}

bool BaseVolume::muted() const
{
    return m_muted;
}

qreal BaseVolume::leftScale() const
{
    return m_scaleLeft;
}

qreal BaseVolume::rightScale() const
{
    return m_scaleRight;
}
} //namespace PlayBackend
} //namespace PhoenixPlayer
