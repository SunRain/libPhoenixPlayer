#include "UserInterface/UserInterfaceMgr.h"

#include "private/UserInterfaceMgrInternal.h"
#include "private/SingletonObjectFactory.h"

namespace PhoenixPlayer {
namespace UserInterface {

UserInterfaceMgr::UserInterfaceMgr(QObject *parent)
    : QObject(parent)
{
    m_internal = SingletonObjectFactory::instance()->userInterfaceInternal();
}

UserInterfaceMgr::~UserInterfaceMgr()
{

}

IUserInterface *UserInterfaceMgr::usedInterface() const
{
    return m_internal.data()->usedInterface();

}

void UserInterfaceMgr::setUsedInterface(const PluginMetaData &data)
{
    m_internal->setUsedInterface(data.libraryFile);
}

void UserInterfaceMgr::setUsedInterface(const QString &libraryFile)
{
    m_internal->setUsedInterface(libraryFile);
}

PluginMetaData UserInterfaceMgr::usedPluginMetaData() const
{
    return m_internal->usedPluginMetaData();
}

void UserInterfaceMgr::reload()
{
    m_internal->reload();
}



} // namespace UserInterface
} // namespace PhoenixPlayer
