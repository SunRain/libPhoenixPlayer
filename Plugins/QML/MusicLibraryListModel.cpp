#include "MusicLibraryListModel.h"

#include <QMetaEnum>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

#include "MusicLibraryManager.h"
#include "Common.h"
#include "Util.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;

namespace QmlPlugin {

//const static char *UNKNOW_TEXT = "Empty!!";

MusicLibraryListModel::MusicLibraryListModel(QAbstractListModel *parent) :
    QAbstractListModel(parent)
{
#ifdef SAILFISH_OS
    mMusicLibraryManager = MusicLibraryManager::instance();
#endif
}

MusicLibraryListModel::~MusicLibraryListModel()
{
    if (!mSongHashList.isEmpty())
        mSongHashList.clear();
}

void MusicLibraryListModel::showAllTracks()
{
    qDebug()<<__FUNCTION__;
    clear();
    mSongHashList = mMusicLibraryManager
            ->querySongMetaElement(Common::E_Hash, QString(), false);
    appendToModel();
}


int MusicLibraryListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mSongHashList.size();
}

QVariant MusicLibraryListModel::data(const QModelIndex &index, int role) const
{
    if (index.row () < 0 || index.row () >= mSongHashList.size ())
        return QVariant();

    QString hash = mSongHashList.at(index.row());
    switch (role) {
    case ModelRoles::RoleAlbumImageUrl:
        return mMusicLibraryManager->queryOne(hash, Common::E_AlbumImageUrl, true);
    case ModelRoles::RoleAlbumName:
        return mMusicLibraryManager->queryOne(hash, Common::E_AlbumName, true);
    case ModelRoles::RoleArtistImageUri:
        return mMusicLibraryManager->queryOne(hash, Common::E_ArtistImageUri, true);
    case ModelRoles::RoleArtistName:
        return mMusicLibraryManager->queryOne(hash, Common::E_ArtistName, true);
    case ModelRoles::RoleCoverArtLarge:
        return mMusicLibraryManager->queryOne(hash, Common::E_CoverArtLarge, true);
    case ModelRoles::RoleCoverArtMiddle:
        return mMusicLibraryManager->queryOne(hash, Common::E_CoverArtMiddle, true);
    case ModelRoles::RoleCoverArtSmall:
        return mMusicLibraryManager->queryOne(hash, Common::E_CoverArtSmall, true);
    case ModelRoles::RoleFileName:
        return mMusicLibraryManager->queryOne(hash, Common::E_FileName, true);
    case ModelRoles::RoleFilePath:
        return mMusicLibraryManager->queryOne(hash, Common::E_FilePath, true);
    case ModelRoles::RoleGenre:
        return mMusicLibraryManager->queryOne(hash, Common::E_Genre, true);
    case ModelRoles::RoleMediaType:
        return mMusicLibraryManager->queryOne(hash, Common::E_MediaType, true);
    case ModelRoles::RoleSongTitle:
        return mMusicLibraryManager->queryOne(hash, Common::E_SongTitle, true);
    case ModelRoles::RoleUserRating:
        return mMusicLibraryManager->queryOne(hash, Common::E_UserRating, true);
    case ModelRoles::RoleHash:
        return hash;
    case ModelRoles::RoleTrackTitle: {
//        QString t = mMusicLibraryManager->queryOne(hash, Common::E_SongTitle);
//        if (t.isEmpty()) {
//            t = mMusicLibraryManager->queryOne(hash, Common::E_FileName);
//            if (!t.isEmpty())
//                t = t.mid(0, t.lastIndexOf("."));
//        }
//        return t;
        return mMusicLibraryManager->querySongTitle(hash);
    }
    case ModelRoles::RoleTrackSubTitle: {
        QString s = mMusicLibraryManager->queryOne(hash, Common::E_AlbumName);
        if (!s.isEmpty())
            s += " - ";
        s += mMusicLibraryManager->queryOne(hash, Common::E_ArtistName);
        QString t = mMusicLibraryManager->queryOne(hash, Common::E_SongLength);
        if (!t.isEmpty())
            s += QString(" (%1)").arg(Util::formateSongDuration(t.toInt()));
        return s;
    }
    case ModelRoles::RoleTrackImageUri: {
//        QString uri = mMusicLibraryManager->queryOne(hash, Common::E_CoverArtMiddle);
//        if (uri.isEmpty())
//            uri = mMusicLibraryManager->queryOne(hash, Common::E_CoverArtLarge);
//        if (uri.isEmpty())
//            uri = mMusicLibraryManager->queryOne(hash, Common::E_CoverArtSmall);
//        if (uri.isEmpty())
//            uri = mMusicLibraryManager->queryOne(hash, Common::E_AlbumImageUrl);
//        if (uri.isEmpty())
//            uri = mMusicLibraryManager->queryOne(hash, Common::E_ArtistImageUri);
//        return uri;
        return mMusicLibraryManager->querySongImageUri(hash);
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MusicLibraryListModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role.insert(ModelRoles::RoleAlbumImageUrl, "albumImageUrl");
    role.insert(ModelRoles::RoleAlbumName, "albumName");
    role.insert(ModelRoles::RoleArtistImageUri, "artistImageUri");
    role.insert(ModelRoles::RoleArtistName, "artistName");
    role.insert(ModelRoles::RoleCoverArtLarge, "coverArtLarge");
    role.insert(ModelRoles::RoleCoverArtMiddle, "coverArtMiddle");
    role.insert(ModelRoles::RoleCoverArtSmall, "coverArtSmall");
    role.insert(ModelRoles::RoleFileName, "fileName");
    role.insert(ModelRoles::RoleFilePath, "filePath");
    role.insert(ModelRoles::RoleGenre, "genre");
    role.insert(ModelRoles::RoleHash, "hash");
    role.insert(ModelRoles::RoleMediaType, "mediaType");
    role.insert(ModelRoles::RoleSongTitle, "songTitle");
    role.insert(ModelRoles::RoleUserRating, "userRating");
    role.insert(ModelRoles::RoleTrackTitle, "trackTitle");
    role.insert(ModelRoles::RoleTrackSubTitle, "trackSubTitle");
    role.insert(ModelRoles::RoleTrackImageUri, "trackImageUri");
    return role;;
}

void MusicLibraryListModel::clear()
{
    beginResetModel();
    mSongHashList.clear();
    endResetModel();
}

void MusicLibraryListModel::appendToModel()
{
    if (mSongHashList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" hash list is empty";
        return;
    }

    for (int i=0; i<mSongHashList.size (); ++i) {
        beginInsertRows (QModelIndex(), i, i);
        endInsertRows ();
    }
}

//QString MusicLibraryListModel::queryOne(const QString &hash,
//                                        Common::SongMetaTags tag,
//                                        bool skipDuplicates) const
//{
//    if (hash.isEmpty())
//        return QString();
//    QStringList list = mMusicLibraryManager
//            ->querySongMetaElement(tag, hash, skipDuplicates);
//    if (list.isEmpty())
//        return QString();
//    return list.first();
//}

} //QmlPlugin
} //PhoenixPlayer
