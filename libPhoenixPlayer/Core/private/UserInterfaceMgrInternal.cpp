#include "UserInterfaceMgrInternal.h"

#include "Logger.h"
#include "PluginMgrInternal.h"
#include "PluginMgr.h"
#include "UserInterface/IUserInterface.h"

namespace PhoenixPlayer {

UserInterfaceMgrInternal::UserInterfaceMgrInternal(QSharedPointer<PluginMgrInternal> pluginMgr,
                                                   QObject *parent)
    : QObject(parent),
      m_pluginMgr(pluginMgr)
{
    reload();
    initFallbackUI();
}

UserInterfaceMgrInternal::~UserInterfaceMgrInternal()
{

}

UserInterface::IUserInterface *UserInterfaceMgrInternal::usedInterface() const
{
    return m_usedInterface;
}

PluginMetaData UserInterfaceMgrInternal::usedPluginMetaData() const
{
    return m_usedPluginMeta;
}

void UserInterfaceMgrInternal::setUsedInterface(const QString &libraryFile)
{
    PluginMetaData data;
    foreach (const auto &it, m_pluginList) {
        if (it.libraryFile == libraryFile) {
            data = it;
            break;
        }
    }
    if (!PluginMetaData::isValid(data)) {
        LOG_WARNING()<<"No valid plugin found for current library file "<<libraryFile;
        return;
    }
    if (PluginMetaData::isValid(m_usedPluginMeta)) {
        m_usedPluginMeta.enabled = false;
        m_pluginMgr->update(m_usedPluginMeta);
        PluginMgr::unload(m_usedPluginMeta);
    }
    m_usedPluginMeta = data;
    m_usedPluginMeta.enabled = true;
    m_pluginMgr->update(m_usedPluginMeta);

    m_usedInterface = qobject_cast<UserInterface::IUserInterface*>(PluginMgr::instance(m_usedPluginMeta));

    reload();
}

void UserInterfaceMgrInternal::reload()
{
    m_pluginList.clear();
    m_pluginList = m_pluginMgr->pluginMetaDataList(BasePlugin::PluginUserInterface);
}

void UserInterfaceMgrInternal::initFallbackUI()
{
    if (m_pluginList.isEmpty()) {
        LOG_WARNING()<<"No user interface plugin found !!";
        return;
    }
    foreach (auto it, m_pluginList) {
        if (!PluginMetaData::isValid(it)) {
            continue;
        }
        if (it.enabled) {
            m_usedInterface
                    = qobject_cast<UserInterface::IUserInterface*>(PluginMgr::instance(it));
            if (!m_usedInterface) {
                it.enabled = false;
                m_usedInterface = Q_NULLPTR;
                m_pluginMgr->update(it);
                continue;
            }
            m_usedPluginMeta = it;
            break;
        }
    }
    if (!PluginMetaData::isValid(m_usedPluginMeta)) {
        foreach (auto it, m_pluginList) {
            if (!PluginMetaData::isValid(it)) {
                continue;
            }
            m_usedInterface
                    = qobject_cast<UserInterface::IUserInterface*>(PluginMgr::instance(it));
            if (!m_usedInterface) {
                m_usedInterface = Q_NULLPTR;
                continue;
            }
            it.enabled = true;
            m_pluginMgr->update(it);
            m_usedPluginMeta = it;
            break;
        }
    }
    reload();
}

} // namespace PhoenixPlayer
