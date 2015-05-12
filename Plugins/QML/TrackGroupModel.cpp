#include "TrackGroupModel.h"

#include <QStringList>
#include <QList>
#include <QVariant>
#include <QDebug>

#include "SingletonPointer.h"
#include "MusicLibrary/MusicLibraryManager.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;

namespace QmlPlugin {

TrackGroupModel::TrackGroupModel(QAbstractListModel *parent)
    : QAbstractListModel(parent)
{
#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
    mMusicLibraryManager = MusicLibraryManager::instance();
#else
    mMusicLibraryManager = SingletonPointer<MusicLibraryManager>::instance ();
#endif
}

TrackGroupModel::~TrackGroupModel()
{
    qDebug()<<__FUNCTION__;
}

void TrackGroupModel::setModelType(TrackGroupModel::ModelType type)
{
    if (mModelType != type)
        emit modelTypeChanged();
    mModelType = type;
    clear();
    queryData();
    appendToModel();
}

TrackGroupModel::ModelType TrackGroupModel::getModelType()
{
    return mModelType;
}

QStringList TrackGroupModel::trackHashListFromGroup(TrackGroupModel::ModelType tragetType, const QString &groupName)
{
    Common::SongMetaTags regColum;
    switch (tragetType) {
    case ModelType::TypeAlbum:
    default:
        regColum = Common::E_AlbumName;
        break;
    case ModelType::TypeArtist:
        regColum = Common::E_ArtistName;
        break;
    case ModelType::TypeFolders:
        regColum = Common::E_FilePath;
        break;
    case ModelType::TypeGenre:
        regColum = Common::E_Genre;
        break;
    case ModelType::TypeMediaType:
        regColum = Common::E_MediaType;
        break;
    case ModelType::TypeUserRating:
        regColum = Common::E_UserRating;
        break;
    }
    return mMusicLibraryManager->queryMusicLibrary (Common::E_Hash, regColum, groupName);
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
    case ModelRoles::RoleImageUri:
        return queryGroupImageUri(mDataList.at(index.row()));
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TrackGroupModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert(ModelRoles::RoleGroupName, "groupName");
    role.insert(ModelRoles::RoleImageUri, "imageUri");
    return role;;
}

void TrackGroupModel::queryData()
{
    switch (mModelType) {
    case ModelType::TypeAlbum:
        mSongMetaTag = Common::E_AlbumName;
        break;
    case ModelType::TypeArtist:
        mSongMetaTag = Common::E_ArtistName;
        break;
    case ModelType::TypeFolders:
        mSongMetaTag = Common::E_FilePath;
        break;
    case ModelType::TypeGenre:
        mSongMetaTag = Common::E_Genre;
        break;
    case ModelType::TypeMediaType:
        mSongMetaTag = Common::E_MediaType;
        break;
    case ModelType::TypeUserRating:
        mSongMetaTag = Common::E_UserRating;
        break;
    default:
        break;
    }
     mDataList = mMusicLibraryManager->querySongMetaElement(mSongMetaTag, QString(), true);
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

QVariant TrackGroupModel::queryGroupImageUri(const QString &groupName) const
{
    QStringList list;
    Common::SongMetaTags tag;
    switch (mModelType) {
    case ModelType::TypeAlbum:
        list = mMusicLibraryManager->queryMusicLibrary(Common::E_AlbumImageUrl, mSongMetaTag, groupName, true);
        tag = Common::E_AlbumImageUrl;
        break;
    case ModelType::TypeArtist:
        list = mMusicLibraryManager->queryMusicLibrary(Common::E_ArtistImageUri, mSongMetaTag, groupName, true);
        tag = Common::E_ArtistImageUri;
        break;
    default:
        return QString();
    }

    //因为当使用 select DISTINCT时候，空值也会被作为""元素加入到QStringList里面，所以此处去掉“”值
    auto checkList = [] (QStringList &list) {
        foreach (QString str, list) {
            if (str.isEmpty())
                list.removeOne(str);
        }
        return list;
    };

    list = checkList(list);

    //循环查询图片地址
    if (list.isEmpty()) {
        QList<Common::SongMetaTags> tagList;
        tagList.append(Common::E_AlbumImageUrl);
        tagList.append(Common::E_ArtistImageUri);
        tagList.append(Common::E_CoverArtMiddle);
        tagList.append(Common::E_CoverArtSmall);
        tagList.append(Common::E_CoverArtLarge);
        for (int i = 0; i < tagList.size(); ++i) {
            if (tagList[i] == tag)
                continue;
            list = mMusicLibraryManager->queryMusicLibrary(tagList[i], mSongMetaTag, groupName, true);
            list = checkList(list);
            if (!list.isEmpty())
                break;
        }
    }
    list = checkList(list);
    if (list.isEmpty())
        return QString();
    return list.first();
}







} //QmlPlugin
} //PhoenixPlayer
