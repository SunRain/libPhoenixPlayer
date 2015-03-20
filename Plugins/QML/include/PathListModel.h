#ifndef PATHLISTMODEL_H
#define PATHLISTMODEL_H

#include <QAbstractListModel>
#include <QDir>
#include <QStringList>


namespace PhoenixPlayer {
namespace QmlPlugin {
class PathListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString basePath READ basePath WRITE setBasePath NOTIFY basePathChanged)
    Q_PROPERTY(bool limitDepth READ limitDepth WRITE setLimitDepth NOTIFY limitDepthChanged)
    Q_PROPERTY(QString currePath READ getCurrentPath NOTIFY currentPathChanged)
public:
    explicit PathListModel(QAbstractListModel *parent = 0);
    virtual ~PathListModel();

    Q_INVOKABLE void jumpTo(const QString &newPath, bool isAbstractPath = false);
    Q_INVOKABLE void cdUp();

    QString getCurrentPath();

    QString basePath();
    void setBasePath(const QString &path);

    bool limitDepth();
    void setLimitDepth(bool limit);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    enum ModelRoles {
        RolePath = Qt::UserRole + 1
    };

signals:
    void basePathChanged();
    void limitDepthChanged();
    void currentPathChanged();

private:
    void appendList();
    void clear();
private:
    QString mBasePath;
    QString mCurrentPath;
    QStringList mPathItemList;
    bool mLimitDepath;
    QDir mDir;

};

} //QmlPlugin
} //PhoenixPlayer
#endif // PATHLISTMODEL_H
