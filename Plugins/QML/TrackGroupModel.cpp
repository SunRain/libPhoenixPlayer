#include "TrackGroupModel.h"

#include <QStringList>
#include <QVariant>
#include <QDebug>

#include "MusicLibraryManager.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;

namespace QmlPlugin {

TrackGroupModel::TrackGroupModel(QAbstractListModel *parent)
    : QAbstractListModel(parent)
{
#ifdef SAILFISH_OS
    mMusicLibraryManager = MusicLibraryManager::instance();
#endif
}

TrackGroupModel::~TrackGroupModel()
{

}

void TrackGroupModel::setModelType(TrackGroupModel::ModelType type)
{
    if (mModelType != type)
        emit modelTypeChanged();
    mModelType = type;
    clear();
    queryData();
}

TrackGroupModel::ModelType TrackGroupModel::getModelType()
{
    return mModelType;
}

int TrackGroupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mDataList.size();
}

QVariant TrackGroupModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= mDataList.size ())
        return QVariant();
    switch (role) {
    case ModelRoles::RoleGroupName:
        return mDataList.at(index.row());
//    case ModelRoles::RoleArtist:
//        return mMusicLibraryManager->querySongMetaElement(Common::E_ArtistName, QString(), true);
//    case ModelRoles::RoleAlbum:
//        return mMusicLibraryManager->querySongMetaElement(Common::E_AlbumName, QString(), true);
//    case ModelRoles::RoleFolders:
//        return mMusicLibraryManager->querySongMetaElement(Common::E_FilePath, QString(), true);
//    case ModelRoles::RoleGenre:
//        return mMusicLibraryManager->querySongMetaElement(Common::E_Genre, QString(), true);
//    case ModelRoles::RoleMediaType:
//        return mMusicLibraryManager->querySongMetaElement(Common::E_MediaType, QString(), true);
//    case ModelRoles::RoleUserRating:
//        return mMusicLibraryManager->querySongMetaElement(Common::E_UserRating, QString(), true);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TrackGroupModel::roleNames() const
{
    QHash<int, QByteArray> role;
//    role.insert(ModelRoles::RoleArtist, "artist");
//    role.insert(ModelRoles::RoleAlbum, "album");
//    role.insert(ModelRoles::RoleGenre, "genre");
//    role.insert(ModelRoles::RoleMediaType, "mediaType");
//    role.insert(ModelRoles::RoleUserRating, "userRating");
//    role.insert(ModelRoles::RoleFolders, "folders");
    role.insert(ModelRoles::RoleGroupName, "groupName");
    return role;;
}

void TrackGroupModel::queryData()
{
    switch (mModelType) {
    case ModelType::TypeAlbum:
        mDataList = mMusicLibraryManager->querySongMetaElement(Common::E_AlbumName, QString(), true);
        break;
    case ModelType::TypeArtist:
        mDataList = mMusicLibraryManager->querySongMetaElement(Common::E_ArtistName, QString(), true);
        break;
    case ModelType::TypeFolders:
        mDataList = mMusicLibraryManager->querySongMetaElement(Common::E_FilePath, QString(), true);
        break;
    case ModelType::TypeGenre:
        mDataList = mMusicLibraryManager->querySongMetaElement(Common::E_Genre, QString(), true);
        break;
    case ModelType::TypeMediaType:
        mDataList = mMusicLibraryManager->querySongMetaElement(Common::E_MediaType, QString(), true);
        break;
    case ModelType::TypeUserRating:
        mDataList = mMusicLibraryManager->querySongMetaElement(Common::E_UserRating, QString(), true);
        break;
    default:
        break;
    }
}

void TrackGroupModel::appendToModel()
{
    if (mDataList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" hash list is empty";
        return;
    }

    for (int i=0; i<mDataList.size (); ++i) {
        beginInsertRows (QModelIndex(), i, i);
        endInsertRows ();
    }
}

void TrackGroupModel::clear()
{
    beginResetModel();
    mDataList.clear();
    endResetModel();
}
} //QmlPlugin
} //PhoenixPlayer
