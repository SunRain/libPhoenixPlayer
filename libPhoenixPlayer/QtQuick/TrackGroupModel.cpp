#include "TrackGroupModel.h"

#include <QStringList>
#include <QList>
#include <QVariant>
#include <QDebug>
#include <AudioMetaObject.h>

#include "SingletonPointer.h"
#include "LibPhoenixPlayerMain.h"

#include "MusicLibrary/MusicLibraryManager.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;

namespace QmlPlugin {

TrackGroupModel::TrackGroupModel(QAbstractListModel *parent)
    : QAbstractListModel(parent)
{
    m_musicLibraryManager = phoenixPlayerLib->libraryMgr ();/*MusicLibraryManager::instance ();*/
}

TrackGroupModel::~TrackGroupModel()
{
    qDebug()<<Q_FUNC_INFO;
}

//void TrackGroupModel::setModelType(TrackGroupModel::ModelType type)
//{
//    if (m_modelType != type)
//        emit modelTypeChanged();
//    m_modelType = type;
//    clear();
//    queryData();
//    appendToModel();
//}

//TrackGroupModel::ModelType TrackGroupModel::getModelType()
//{
//    return m_modelType;
//}

//QStringList TrackGroupModel::trackHashListFromGroup(TrackGroupModel::ModelType tragetType, const QString &groupName)
//{
//    Common::SongMetaTags regColum;
//    switch (tragetType) {
//    case ModelType::TypeAlbum:
//    default:
//        regColum = Common::E_AlbumName;
//        break;
//    case ModelType::TypeArtist:
//        regColum = Common::E_ArtistName;
//        break;
//    case ModelType::TypeFolders:
//        regColum = Common::E_FilePath;
//        break;
//    case ModelType::TypeGenre:
//        regColum = Common::E_Genre;
//        break;
//    case ModelType::TypeMediaType:
//        regColum = Common::E_MediaType;
//        break;
//    case ModelType::TypeUserRating:
//        regColum = Common::E_UserRating;
//        break;
//    }
//    return m_musicLibraryManager->queryMusicLibrary (Common::E_Hash, regColum, groupName);
//}

int TrackGroupModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_groupList.size ();
}

QVariant TrackGroupModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= m_groupList.size ())
        return QVariant();
    switch (role) {
    case ModelRoles::RoleGroupName:
        return m_groupList.at (index.row ()).name;
    case ModelRoles::RoleImageUri:
        return m_groupList.at (index.row ()).imgUri;
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

TrackGroupModel::ModelType TrackGroupModel::type() const
{
    return m_type;
}

void TrackGroupModel::queryData()
{
//    switch (m_modelType) {
//    case ModelType::TypeAlbum:
//        mSongMetaTag = Common::E_AlbumName;
//        break;
//    case ModelType::TypeArtist:
//        mSongMetaTag = Common::E_ArtistName;
//        break;
//    case ModelType::TypeFolders:
//        mSongMetaTag = Common::E_FilePath;
//        break;
//    case ModelType::TypeGenre:
//        mSongMetaTag = Common::E_Genre;
//        break;
//    case ModelType::TypeMediaType:
//        mSongMetaTag = Common::E_MediaType;
//        break;
//    case ModelType::TypeUserRating:
//        mSongMetaTag = Common::E_UserRating;
//        break;
//    default:
//        break;
//    }
//     mDataList = m_musicLibraryManager->querySongMetaElement(mSongMetaTag, QString(), true);
//    QList<QObject *> list = m_musicLibraryManager->allTracks ();
    AudioMetaList list = m_musicLibraryManager->allTracks ();
    foreach (AudioMetaObject d, list) {
//        AudioMetaObject *d = qobject_cast<AudioMetaObject *>(o);
        if (d.isEmpty ())
            continue;
        switch (m_type) {
        case ModelType::TypeAlbum: {
            GroupObject obj;
            obj.name = d.albumMeta ().name ();//d->albumMeta ()->name ();
            obj.imgUri = d.albumMeta ().imgUri ();//d->albumMeta ()->imgUri ();
            if (!m_groupList.contains (obj))
                m_groupList.append (obj);
            break;
        }
        case ModelType::TypeArtist: {
            GroupObject obj;
            obj.name = d.artistMeta ().name ();//d->artistMeta ()->name ();
            obj.imgUri = d.artistMeta ().imgUri ();//d->artistMeta ()->imgUri ();
            if (!m_groupList.contains (obj))
                m_groupList.append (obj);
            break;
        }
        case ModelType::TypeFolders: {
            GroupObject obj;
            obj.name = d.path ();//d->path ();
            obj.imgUri = d.coverMeta ().middleUri ();//d->coverMeta ()->middleUri ();
            if (!m_groupList.contains (obj))
                m_groupList.append (obj);
            break;
        }
        case ModelType::TypeGenre: {
            GroupObject obj;
            obj.name = d.trackMeta ().genre ();//d->trackMeta ()->genre ().toString ();
            obj.imgUri = d.coverMeta ().middleUri ();//d->coverMeta ()->middleUri ();
            if (!m_groupList.contains (obj))
                m_groupList.append (obj);
            break;
        }
        case ModelType::TypeMediaType: {
            GroupObject obj;
            obj.name = d.mediaType ();//d->mediaType ();
            obj.imgUri = d.coverMeta ().middleUri ();// d->coverMeta ()->middleUri ();
            if (!m_groupList.contains (obj))
                m_groupList.append (obj);
            break;
        }
        case ModelType::TypeUserRating: {
            GroupObject obj;
            obj.name = d.trackMeta ().userRating ();//d->trackMeta ()->userRating ().toString ();
            obj.imgUri = d.coverMeta ().middleUri ();//d->coverMeta ()->middleUri ();
            if (!m_groupList.contains (obj))
                m_groupList.append (obj);
            break;
        }
        default:
            break;
        }
    }

    ///如果某个groupobject的imguri是空值，则从m_groupList循环读取某一个相同的GroupObject.name值来补全
    foreach (GroupObject obj, m_groupList) {
        if (obj.imgUri.isEmpty ()) {
            foreach (/*QObject *o*/AudioMetaObject d, list) {
//                AudioMetaObject *d = qobject_cast<AudioMetaObject *>(o);
                if (d.isEmpty ())
                    continue;
                if (m_type == ModelType::TypeAlbum) {
                    if (d.albumMeta ().name () == obj.name
                            && (!d.albumMeta ().imgUri ().isEmpty ())) {
                        obj.imgUri = d.albumMeta ().imgUri ();
                        break;
                    }
                } else if(m_type == ModelType::TypeArtist) {
                    if (d.artistMeta ().name () == obj.name
                            && (!d.artistMeta ().imgUri ().isEmpty ())) {
                        obj.imgUri = d.artistMeta ().imgUri ();
                        break;
                    }
                } else if(m_type == ModelType::TypeFolders) {
                    if (d.path ()== obj.name
                            && (!d.coverMeta ().middleUri ().isEmpty ())) {
                        obj.imgUri = d.coverMeta ().middleUri ();
                        break;
                    }
                } else if(m_type == ModelType::TypeGenre) {
                    if (d.trackMeta ().genre () == obj.name
                            && (!d.coverMeta ().middleUri ().isEmpty ())) {
                        obj.imgUri = d.coverMeta ().middleUri ();
                        break;
                    }
                } else if(m_type == ModelType::TypeMediaType) {
                    if (QString::number (d.mediaType ()) == obj.name
                            && (!d.coverMeta ().middleUri ().isEmpty ())) {
                        obj.imgUri = d.coverMeta ().middleUri ();
                        break;
                    }
                } else if(m_type == ModelType::TypeUserRating) {
                    if (d.trackMeta ().userRating () == obj.name
                            && (!d.coverMeta ().middleUri ().isEmpty ())) {
                        obj.imgUri = d.coverMeta ().middleUri ();
                        break;
                    }
                }
            }
        }
    }
}

void TrackGroupModel::appendToModel()
{
    if (m_groupList.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"group list empty";
        return;
    }
    for (int i=0; i<m_groupList.size (); ++i) {
        beginInsertRows (QModelIndex(), i, i);
        endInsertRows ();
    }
}

void TrackGroupModel::clear()
{
    beginResetModel();
    m_groupList.clear ();
    endResetModel();
}

void TrackGroupModel::setType(TrackGroupModel::ModelType type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged(type);
    }
    clear();
    queryData();
    appendToModel();
}

//QVariant TrackGroupModel::queryGroupImageUri(const QString &groupName) const
//{
//    QStringList list;
//    Common::SongMetaTags tag;
//    switch (m_modelType) {
//    case ModelType::TypeAlbum:
//        list = m_musicLibraryManager->queryMusicLibrary(Common::E_AlbumImageUrl, mSongMetaTag, groupName, true);
//        tag = Common::E_AlbumImageUrl;
//        break;
//    case ModelType::TypeArtist:
//        list = m_musicLibraryManager->queryMusicLibrary(Common::E_ArtistImageUri, mSongMetaTag, groupName, true);
//        tag = Common::E_ArtistImageUri;
//        break;
//    default:
//        return QString();
//    }

//    //因为当使用 select DISTINCT时候，空值也会被作为""元素加入到QStringList里面，所以此处去掉“”值
//    auto checkList = [] (QStringList &list) {
//        foreach (QString str, list) {
//            if (str.isEmpty())
//                list.removeOne(str);
//        }
//        return list;
//    };

//    list = checkList(list);

//    //循环查询图片地址
//    if (list.isEmpty()) {
//        QList<Common::SongMetaTags> tagList;
//        tagList.append(Common::E_AlbumImageUrl);
//        tagList.append(Common::E_ArtistImageUri);
//        tagList.append(Common::E_CoverArtMiddle);
//        tagList.append(Common::E_CoverArtSmall);
//        tagList.append(Common::E_CoverArtLarge);
//        for (int i = 0; i < tagList.size(); ++i) {
//            if (tagList[i] == tag)
//                continue;
//            list = m_musicLibraryManager->queryMusicLibrary(tagList[i], mSongMetaTag, groupName, true);
//            list = checkList(list);
//            if (!list.isEmpty())
//                break;
//        }
//    }
//    list = checkList(list);
//    if (list.isEmpty())
//        return QString();
//    return list.first();
//}

} //QmlPlugin
} //PhoenixPlayer
