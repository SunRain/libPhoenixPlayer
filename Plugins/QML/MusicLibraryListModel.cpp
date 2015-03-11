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
    mLimitNum = -1;
}

MusicLibraryListModel::~MusicLibraryListModel()
{
    if (!mSongHashList.isEmpty())
        mSongHashList.clear();
}

void MusicLibraryListModel::showAllTracks()
{
    clear();
    mSongHashList = mMusicLibraryManager
            ->querySongMetaElement(Common::E_Hash, QString(), false);
    appendToModel();
}

void MusicLibraryListModel::showFolderTracks(const QString &folder, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_FilePath, folder);
    appendToModel(limitNum);
}

void MusicLibraryListModel::showUserRatingTracks(const QString &rating, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_UserRating, rating);
    appendToModel(limitNum);
}

void MusicLibraryListModel::showMediaTypeTracks(const QString &mediaType, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_MediaType, mediaType);
    appendToModel(limitNum);
}

void MusicLibraryListModel::showGenreTracks(const QString &genreName, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_Genre, genreName);
    appendToModel(limitNum);
}

void MusicLibraryListModel::showAlbumTracks(const QString &albumName, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_AlbumName, albumName);
    appendToModel(limitNum);
}

void MusicLibraryListModel::showArtistTracks(const QString &artistName, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager->queryMusicLibrary(Common::E_Hash, Common::E_ArtistName, artistName);
    appendToModel(limitNum);
}


int MusicLibraryListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (mLimitNum >=0)
        return qMin(mLimitNum, mSongHashList.size());
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

void MusicLibraryListModel::appendToModel(int limitNum)
{
    if (mSongHashList.isEmpty()) {
        qDebug()<<__FUNCTION__<<" hash list is empty";
        return;
    }

    for (int i=0; i<mSongHashList.size (); ++i) {
        if (limitNum > 0 && i >= limitNum) {
            break;
        }
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
