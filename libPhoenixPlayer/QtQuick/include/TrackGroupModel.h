#ifndef TRACKGROUPMODEL_H
#define TRACKGROUPMODEL_H

#include <QAbstractListModel>
#include <QUrl>

#include "Common.h"

class QByteArray;
class QVariant;
class QStringList;
namespace PhoenixPlayer {

namespace MusicLibrary {
class MusicLibraryManager;
}
namespace QmlPlugin {

class TrackGroupModel : public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(ModelType)
    Q_PROPERTY(ModelType type READ type WRITE setType NOTIFY typeChanged)
public:
    struct GroupObject {
        QUrl imgUri;
        QString name;
        bool operator ==(const GroupObject &other) {
            return this->name == other.name;
        }
    };

    enum ModelType {
        TypeArtist = 0x0,
        TypeAlbum,
        TypeMediaType,
        TypeUserRating,
        TypeGenre,
        TypeFolders
    };

    explicit TrackGroupModel(QAbstractListModel *parent = 0);
    virtual ~TrackGroupModel();

//    void setModelType(ModelType type);
//    ModelType getModelType();
    ModelType type() const;

//    Q_INVOKABLE QStringList trackHashListFromGroup(ModelType tragetType, const QString &groupName);

public:
    enum ModelRoles {
        RoleGroupName = Qt::UserRole + 1,
        RoleImageUri
    };
    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

protected:
    void queryData();
    void appendToModel();
signals:
    void typeChanged(ModelType type);

public slots:
    void clear();
    void setType(ModelType type);
private:
//    QVariant queryGroupImageUri(const QString &groupName) const;
private:
    MusicLibrary::MusicLibraryManager *m_musicLibraryManager;
//    QStringList mDataList;
//    ModelType m_modelType;
    //    Common::SongMetaTags mSongMetaTag;
    QList<GroupObject> m_groupList;
    ModelType m_type;
};


} //QmlPlugin
} //PhoenixPlayer

#endif // TRACKGROUPMODEL_H
