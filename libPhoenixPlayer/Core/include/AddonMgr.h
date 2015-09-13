#ifndef ADDONMGR_H
#define ADDONMGR_H

#include <QObject>
#include <QList>

namespace PhoenixPlayer {

class AddonMgr : public QObject
{
    Q_OBJECT
public:
    explicit AddonMgr(QObject *parent = 0);
    virtual ~AddonMgr();

    struct Addon {
        QString hash;
        QString basePath;
        QString indexFile;
        QString name;
        QString icon;
        QString version;
        QString description;
        QString maintainer;
    };

    QList<Addon *> getAddonList();
protected:
    void initList();
private:
    QList<Addon *> m_addonList;
};
} //PhoenixPlayer
#endif // ADDONMGR_H
