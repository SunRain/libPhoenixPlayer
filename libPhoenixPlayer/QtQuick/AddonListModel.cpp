#include "AddonListModel.h"


#include "AddonMgr.h"

namespace PhoenixPlayer {
namespace QmlPlugin {

AddonListModel::AddonListModel(QAbstractListModel *parent)
    : QAbstractListModel(parent)
{
    mAddonMgr = new AddonMgr(this);
}

AddonListModel::~AddonListModel()
{
    if (mAddonMgr)
        mAddonMgr->deleteLater ();
}

int AddonListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mAddonMgr->getAddonList ().size ();
}

QVariant AddonListModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= mAddonMgr->getAddonList ().size ())
        return QVariant();
    AddonMgr::Addon *addon = mAddonMgr->getAddonList ().at (index.row ());
    switch (role) {
    case ModelRoles::RoleBasePath:
        return addon->basePath;
    case ModelRoles::RoleDescription:
        return addon->description;
    case ModelRoles::RoleHash:
        return addon->hash;
    case ModelRoles::RoleIcon:
        return addon->icon;
    case ModelRoles::RoleIndexFile:
        return addon->indexFile;
    case ModelRoles::RoleMaintainer:
        return addon->maintainer;
    case ModelRoles::RoleName:
        return addon->name;
    case ModelRoles::RoleVersion:
        return addon->version;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> AddonListModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert (ModelRoles::RoleBasePath, "basePath");
    role.insert (ModelRoles::RoleDescription, "description");
    role.insert (ModelRoles::RoleHash, "hash");
    role.insert (ModelRoles::RoleIcon, "icon");
    role.insert (ModelRoles::RoleIndexFile, "indexFile");
    role.insert (ModelRoles::RoleMaintainer, "maintainer");
    role.insert (ModelRoles::RoleName, "name");
    role.insert (ModelRoles::RoleVersion, "version");
    return role;
}

} //QmlPlugin
} //PhoenixPlayer
