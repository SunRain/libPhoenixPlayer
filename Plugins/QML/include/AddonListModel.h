#ifndef ADDONLISTMODEL_H
#define ADDONLISTMODEL_H

#include <QAbstractListModel>

namespace PhoenixPlayer {
class AddonMgr;
namespace QmlPlugin {

class AddonListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AddonListModel(QAbstractListModel *parent = 0);
    virtual ~AddonListModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

public:
    enum ModelRoles {
        RoleHash = Qt::UserRole + 1,
        RoleBasePath,
        RoleIndexFile,
        RoleName,
        RoleIcon,
        RoleVersion,
        RoleDescription,
        RoleMaintainer

    };
signals:

public slots:

private:
    AddonMgr *mAddonMgr;
};
} //QmlPlugin
} //PhoenixPlayer
#endif // ADDONLISTMODEL_H
