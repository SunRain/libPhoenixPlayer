#ifndef PLUGINLISTMODEL_H
#define PLUGINLISTMODEL_H

#include <QAbstractListModel>

namespace PhoenixPlayer {
class PluginLoader;
class PluginHost;
namespace QmlPlugin {
class PluginListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PluginListModel(QObject *parent = 0);
    virtual ~PluginListModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
public:
    enum ModelRoles {
        RoleName = Qt::UserRole + 1,
        RolePluginType,
//        RoleBasePath,
        RoleVersion,
        RoleDescription,
        RoleConfigFile,
        RoleHash
    };
signals:

public slots:

private:
    PluginLoader *mPluginLoader;

};
} //QmlPlugin
} //PhoenixPlayer

#endif // PLUGINLISTMODEL_H
