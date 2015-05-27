#ifndef ADDONMGR_H
#define ADDONMGR_H

#include <QObject>

namespace PhoenixPlayer {

class AddonMgr : public QObject
{
    Q_OBJECT
public:
    explicit AddonMgr(QObject *parent = 0);
    virtual ~AddonMgr();


};
} //PhoenixPlayer
#endif // ADDONMGR_H
