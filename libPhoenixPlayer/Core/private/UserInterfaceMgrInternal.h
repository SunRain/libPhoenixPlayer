#ifndef USERINTERFACEMGRINTERNAL_H
#define USERINTERFACEMGRINTERNAL_H

#include <QObject>

#include "PluginMgr.h"

namespace PhoenixPlayer {

    class PluginMgrInternal;

    namespace UserInterface {
        class IUserInterface;
    }

class UserInterfaceMgrInternal : public QObject
{
    Q_OBJECT
public:
    explicit UserInterfaceMgrInternal(QSharedPointer<PluginMgrInternal> pluginMgr,
                                      QObject *parent = Q_NULLPTR);

    virtual ~UserInterfaceMgrInternal();

    UserInterface::IUserInterface *usedInterface() const;

    PluginMetaData usedPluginMetaData() const;

    void setUsedInterface(const QString &libraryFile);

    void reload();

private:
    void initFallbackUI();

private:
    UserInterface::IUserInterface       *m_usedInterface = Q_NULLPTR;
    QSharedPointer<PluginMgrInternal>   m_pluginMgr;
    PluginMetaData                      m_usedPluginMeta;
    QList<PluginMetaData>               m_pluginList;


};

} //namespace PhoenixPlayer

#endif // USERINTERFACEMGRINTERNAL_H
