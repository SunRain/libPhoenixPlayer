#ifndef EQUALIZERMGR_H
#define EQUALIZERMGR_H

#include <QObject>
#include <QHash>

#include "libphoenixplayer_global.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {

class PPSettings;
class EqualizerMgr : public QObject
{
    Q_OBJECT
private:
    explicit EqualizerMgr(QObject *parent = Q_NULLPTR);
    static EqualizerMgr *createInstance();
public:
    static EqualizerMgr *instance();

    virtual ~EqualizerMgr();

    void setEnabled(bool enable);
    bool enabled() const;

    ///
    /// \brief setPreamp Sets equalizer preamp
    /// \param preamp
    ///
    void setPreamp(double preamp);
    double preamp();

    ///
    /// \brief setValue Sets the equalizer channel value.
    /// \param band  Number of equalizer band.
    /// \param value Channel gain (-20.0..20.0 dB)
    ///
    void setValue(int band, double value);
    double value(int band);

    int bands() const;

signals:
    void changed();
public slots:
    void reload();
    void sync();
private:
    void save();
private:
    PPSettings *m_settings;
    bool m_enabled;
    double m_preamp;
    QHash<int, double> m_values;

};
} //PhoenixPlayer
#endif // EQUALIZERMGR_H
