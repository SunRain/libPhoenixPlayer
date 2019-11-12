#ifndef EQUALIZERMGRINTERNAL_H
#define EQUALIZERMGRINTERNAL_H

#include <QObject>
#include <QSharedPointer>


namespace PhoenixPlayer {

const static int EQUALIZER_BANDS_NUM = 10;

class PPSettingsInternal;
class EqualizerMgrInternal : public QObject
{
    Q_OBJECT
public:
    explicit EqualizerMgrInternal(QSharedPointer<PPSettingsInternal> settings, QObject *parent = nullptr);

    virtual ~EqualizerMgrInternal();

    inline void setEnabled(bool enable)
    {
        m_enabled = enable;
    }
    inline bool enabled() const
    {
        return m_enabled;
    }

    inline void setPreamp(double preamp)
    {
        m_preamp = preamp;
    }
    inline double preamp() const
    {
        return m_preamp;
    }

    inline void setValue(int band, double value)
    {
        m_values.insert(band, value);
    }
    inline double value(int band) const
    {
        return m_values.value(band);
    }

    inline int bands() const
    {
        return EQUALIZER_BANDS_NUM;
    }

    void reload();

    void sync();

signals:
    void changed();

private:
    void save();

private:
    QSharedPointer<PPSettingsInternal> m_settings;
    bool                m_enabled;
    double              m_preamp;
    QHash<int, double>  m_values;
};

} //namespace PhoenixPlayer
#endif // EQUALIZERMGRINTERNAL_H
