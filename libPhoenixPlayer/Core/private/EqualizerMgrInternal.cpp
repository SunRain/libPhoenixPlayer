#include "EqualizerMgrInternal.h"

#include "PPSettingsInternal.h"

namespace PhoenixPlayer {


EqualizerMgrInternal::EqualizerMgrInternal(QSharedPointer<PPSettingsInternal> settings, QObject *parent)
    : QObject(parent),
    m_settings(settings)
{
    reload();
}

EqualizerMgrInternal::~EqualizerMgrInternal()
{
    this->save();
}

void EqualizerMgrInternal::reload()
{
    for (int i=0; i<EQUALIZER_BANDS_NUM; ++i) {
        m_values.insert(i,
                        m_settings->internalSettings()->value(QString("Equalizer/Band_%1").arg(i),
                                                              0).toDouble());
    }
    m_preamp = m_settings->internalSettings()->value("Equalizer/Preamp", 0).toDouble();
    m_enabled = m_settings->internalSettings()->value("Equalizer/Enable", false).toBool();
    emit changed();
}

void EqualizerMgrInternal::sync()
{
    save();
    reload();
}

void EqualizerMgrInternal::save()
{
    auto i = m_values.constBegin();
    while (i != m_values.constEnd()) {
        m_settings->internalSettings()->setValue(QString("Equalizer/Band_%1").arg(i.key()),
                                                 i.value());
        ++i;
    }
    m_settings->internalSettings()->setValue("Equalizer/Preamp", m_preamp);
    m_settings->internalSettings()->setValue("Equalizer/Enable", m_enabled);
    m_settings->internalSettings()->sync();
}

} //namespace PhoenixPlayer
