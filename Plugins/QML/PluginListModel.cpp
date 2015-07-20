#include "PluginListModel.h"

#include <QDebug>

#include "SingletonPointer.h"
#include "PluginLoader.h"
#include "PluginHost.h"
#include "Common.h"

namespace PhoenixPlayer {
namespace QmlPlugin {
PluginListModel::PluginListModel(QObject *parent) :
    QAbstractListModel(parent)
{
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    mSettings = Settings::instance();
    mPluginLoader = PluginLoader::instance();
#else
//    mSettings = SingletonPointer<Settings>::instance ();
    mPluginLoader = SingletonPointer<PluginLoader>::instance ();
#endif
}

PluginListModel::~PluginListModel()
{

}

int PluginListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mPluginLoader->getPluginHostSize (Common::PluginTypeAll);
}

QVariant PluginListModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0
            || index.row () >= mPluginLoader->getPluginHostSize (Common::PluginTypeAll)) {
        return QVariant();
    }
    if (mPluginLoader->getPluginHostList (Common::PluginTypeAll).isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"host size is empty";
        return QVariant();
    }
    PluginHost *h = mPluginLoader->getPluginHostList (Common::PluginTypeAll).at (index.row ());
    switch (role) {
    case ModelRoles::RoleConfigFile:
        return h->configFile ();
    case ModelRoles::RoleDescription:
        return h->description ();
    case ModelRoles::RoleName:
        return h->name ();
    case ModelRoles::RoleVersion:
        return h->version ();
    case ModelRoles::RolePluginType:
        return h->type ();
    case ModelRoles::RoleHash:
        return h->hash ();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PluginListModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert (ModelRoles::RoleConfigFile, "configFile");
    role.insert (ModelRoles::RoleDescription, "description");
    role.insert (ModelRoles::RoleName, "name");
    role.insert (ModelRoles::RoleVersion, "version");
    role.insert (ModelRoles::RolePluginType, "type");
    role.insert (ModelRoles::RoleHash, "hash");
    return role;
}

} //QmlPlugin
} //PhoenixPlayer
