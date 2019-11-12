#include "EqualizerMgr.h"

#include <QDebug>

#include "LibPhoenixPlayerMain.h"
#include "private/EqualizerMgrInternal.h"
#include "private/SingletonObjectFactory.h"

namespace PhoenixPlayer {

EqualizerMgr::EqualizerMgr(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->eqMgrInternal();
    connect(m_internal.data(), &EqualizerMgrInternal::changed,
            this, &EqualizerMgr::changed);
}


EqualizerMgr::~EqualizerMgr()
{
    m_internal->disconnect(this);
}

void EqualizerMgr::setGlobalEnabled(bool enable)
{
    SingletonObjectFactory::instance()->eqMgrInternal()->setEnabled(enable);
}

bool EqualizerMgr::globalEnabled()
{
    return SingletonObjectFactory::instance()->eqMgrInternal()->enabled();
}

void EqualizerMgr::setEnabled(bool enable)
{
    m_internal->setEnabled(enable);
}

bool EqualizerMgr::enabled() const
{
    return m_internal->enabled();
}

void EqualizerMgr::setPreamp(double preamp)
{
    m_internal->setPreamp(preamp);
}

double EqualizerMgr::preamp() const
{
    return m_internal->preamp();
}

void EqualizerMgr::setValue(int band, double value)
{
    m_internal->setValue(band, value);
}

double EqualizerMgr::value(int band) const
{
    return m_internal->value(band);
}

int EqualizerMgr::bands() const
{
    return m_internal->bands();
}

void EqualizerMgr::reload()
{
    m_internal->reload();
}

void EqualizerMgr::sync()
{
    m_internal->sync();
}

} //PhoenixPlayer
