#include "EqualizerMgr.h"

#include <QDebug>

#include "PPSettings.h"
#include "LibPhoenixPlayerMain.h"

namespace PhoenixPlayer {

const static int BANDS_NUM = 10;

EqualizerMgr::EqualizerMgr(QObject *parent)
    : QObject(parent)
{
    m_settings = phoenixPlayerLib->settings ();//Settings::instance ();

    reload ();
}

EqualizerMgr *EqualizerMgr::createInstance()
{
    return new EqualizerMgr();
}

EqualizerMgr *EqualizerMgr::instance()
{
     return Singleton<EqualizerMgr>::instance(EqualizerMgr::createInstance);
}

EqualizerMgr::~EqualizerMgr()
{
    save ();
}

void EqualizerMgr::setEnabled(bool enable)
{
    m_enabled = enable;
}

bool EqualizerMgr::enabled() const
{
    return m_enabled;
}

void EqualizerMgr::setPreamp(double preamp)
{
    m_preamp = preamp;
}

double EqualizerMgr::preamp()
{
    return m_preamp;
}

void EqualizerMgr::setValue(int band, double value)
{
    m_values.insert (band, value);
}

double EqualizerMgr::value(int band)
{
    return m_values.value (band);
}

int EqualizerMgr::bands() const
{
    return BANDS_NUM;
}

void EqualizerMgr::reload()
{
    for (int i=0; i<BANDS_NUM; ++i) {
        m_values.insert (i, m_settings->getConfig (QString("Equalizer/Band_%1").arg (i), QString::number (0)).toDouble ());
    }
    m_preamp = m_settings->getConfig ("Equalizer/Preamp", QString::number (0).toDouble ());
    m_enabled = m_settings->getConfig ("Equalizer/Enable", false);
    emit changed();
}

void EqualizerMgr::sync()
{
    save ();
    reload ();
}

void EqualizerMgr::save()
{
    QHash<int, double>::const_iterator i = m_values.constBegin ();
    while (i != m_values.constEnd ()) {
        m_settings->setConfig (QString("Equalizer/Band_%1").arg (i.key ()),
                               QString::number (i.value ()));
        ++i;
    }
    m_settings->setConfig ("Equalizer/Preamp", QString::number (m_preamp));
    m_settings->setConfig ("Equalizer/Enable", m_enabled);
}

} //PhoenixPlayer
