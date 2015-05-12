#ifndef TRACKGROUPMODEL_H
#define TRACKGROUPMODEL_H

#include <QAbstractListModel>

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
    Q_PROPERTY(ModelType modelType READ getModelType WRITE setModelType NOTIFY modelTypeChanged)
public:
    enum ModelType {
        TypeArtist = 0x0,
        TypeAlbum,
        TypeMediaType,
        TypeUserRating,
        TypeGenre,
        TypeFolders
    };

    explicit TrackGroupModel(QAbstractListModel *parent = 0);
    ~TrackGroupModel();

    void setModelType(ModelType type);
    ModelType getModelType();

    Q_INVOKABLE QStringList trackHashListFromGroup(ModelType tragetType, const QString &groupName);

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
    void modelTypeChanged();
public slots:
    void clear();
private:
    QVariant queryGroupImageUri(const QString &groupName) const;
private:
    MusicLibrary::MusicLibraryManager *mMusicLibraryManager;
    QStringList mDataList;
    ModelType mModelType;
    Common::SongMetaTags mSongMetaTag;
};


} //QmlPlugin
} //PhoenixPlayer

#endif // TRACKGROUPMODEL_H
