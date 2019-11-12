#ifndef EQUALIZERMGR_H
#define EQUALIZERMGR_H

#include <QObject>
#include <QSharedPointer>
#include <QtGlobal>

#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class EqualizerMgrInternal;
/*!
 * \brief The EqualizerMgr class
 * Internal Singleton class
 */
class EqualizerMgr : public QObject
{
    Q_OBJECT
public:
    explicit EqualizerMgr(QObject *parent = Q_NULLPTR);

    virtual ~EqualizerMgr();

    /*!
     * \brief setGlobalEnabled
     * Since EqualizerMgr is an internal Singleton class,
     * any changes to it will also change all instances of it.
     * This is an overloads function to setEnabled, every time call this will call interanl instance function,
     * use setEnabled will be more efficiently.
     * \param enable
     */
    static void setGlobalEnabled(bool enable);
    static bool globalEnabled();

    void setEnabled(bool enable);
    bool enabled() const;

    /*!
     * \brief setPreamp Sets equalizer preamp
     * \param preamp
     */
    void setPreamp(double preamp);
    double preamp() const;

    /*!
     * \brief setValue Sets the equalizer channel value.
     * \param band Number of equalizer band.
     * \param value Channel gain (-20.0..20.0 dB)
     */
    void setValue(int band, double value);
    double value(int band) const;

    int bands() const;

signals:
    void changed();

public slots:
    void reload();

    /*!
     * \brief sync call sync to save data and emit changed() signal if preamp or band changes.
     */
    void sync();

private:
    QSharedPointer<EqualizerMgrInternal> m_internal;

};
} //PhoenixPlayer
#endif // EQUALIZERMGR_H
