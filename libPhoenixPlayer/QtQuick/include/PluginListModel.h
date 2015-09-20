#ifndef PLUGINLISTMODEL_H
#define PLUGINLISTMODEL_H

#include <QAbstractListModel>

#include "Common.h"

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
        RoleVersion,
        RoleDescription,
        RoleConfigFile,
        RoleLibraryFile
    };
signals:

public slots:

private:
    PluginLoader *m_pluginLoader;
//    QHash<Common::PluginType, QStringList> m_libs;
    QList<PluginHost *> m_hosts;

};
} //QmlPlugin
} //PhoenixPlayer

#endif // PLUGINLISTMODEL_H
