#include "MusicLibraryListModel.h"

#include <QMetaEnum>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

#include "MusicLibrary/MusicLibraryManager.h"
#include "MetadataLookup/MetadataLookupMgrWrapper.h"
#include "Common.h"
#include "Util.h"
#include "SingletonPointer.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;
using namespace MetadataLookup;

namespace QmlPlugin {

//const static char *UNKNOW_TEXT = "Empty!!";

MusicLibraryListModel::MusicLibraryListModel(QAbstractListModel *parent) :
    QAbstractListModel(parent)
{
//#if defined(SAILFISH_OS) || defined(UBUNTU_TOUCH)
//    mMusicLibraryManager = MusicLibraryManager::instance();
//    mLookupMgr = MetadataLookupMgrWrapper::instance ();
//#else
//    mMusicLibraryManager = SingletonPointer<MusicLibraryManager>::instance ();
//    mLookupMgr = SingletonPointer<MetadataLookupMgrWrapper>::instance ();
//#endif
    mLimitNum = -1;
    mAutoFetchMetadata = false;
    connect (mLookupMgr, &MetadataLookupMgrWrapper::lookupAlbumImageSucceed,
             [&](const QString &hash, const QString &result) {
        emit trackImageFetched (hash);
    });
    connect (mLookupMgr, &MetadataLookupMgrWrapper::lookupArtistImageSucceed,
             [&](const QString &hash, const QString &result){
        emit trackImageFetched (hash);
    });
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

void MusicLibraryListModel::setAutoFetchMetadata(bool autoFetch)
{
    if (mAutoFetchMetadata == autoFetch)
        return;
    mAutoFetchMetadata = autoFetch;
    emit autoFetchMetadataChanged ();
}

bool MusicLibraryListModel::autoFetchMetadata()
{
    return mAutoFetchMetadata;
}

void MusicLibraryListModel::fetchTrackImage(const QString &hash)
{
    if (hash.isEmpty ())
        return;
    QString str = mMusicLibraryManager->queryOne(hash, Common::E_AlbumImageUrl, true);
    if (str.isEmpty ())
        mLookupMgr->lookupAlbumImage (hash);
    str = mMusicLibraryManager->queryOne(hash, Common::E_ArtistImageUri, true);
    if (str.isEmpty ())
        mLookupMgr->lookupArtistImage (hash);
}

void MusicLibraryListModel::showFolderTracks(const QString &folder, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_FilePath, folder);
    appendToModel(limitNum);
}

void MusicLibraryListModel::showPlayListItems(const QString &playListHash, int limitNum)
{
    mLimitNum = limitNum;
    clear();
    mSongHashList = mMusicLibraryManager
            ->queryPlayListElement (Common::PlayListSongHashes, playListHash);
    appendToModel (limitNum);
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
    mSongHashList = mMusicLibraryManager
            ->queryMusicLibrary(Common::E_Hash, Common::E_ArtistName, artistName);
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
    QString str;
    switch (role) {
    case ModelRoles::RoleAlbumImageUrl: {
        str = mMusicLibraryManager->queryOne(hash, Common::E_AlbumImageUrl, true);
        if (str.isEmpty () && mAutoFetchMetadata)
            mLookupMgr->lookupAlbumImage (hash);
        return str;
    }
    case ModelRoles::RoleAlbumName: {
        return mMusicLibraryManager->queryOne(hash, Common::E_AlbumName, true);
    }
    case ModelRoles::RoleArtistImageUri: {
        str = mMusicLibraryManager->queryOne(hash, Common::E_ArtistImageUri, true);
        if (str.isEmpty () && mAutoFetchMetadata)
            mLookupMgr->lookupArtistImage (hash);
        return str;
    }
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
        return mMusicLibraryManager->queryOne(hash, Common::E_TrackTitle, true);
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
        QString t = mMusicLibraryManager->queryOne(hash, Common::E_TrackLength);
        if (!t.isEmpty())
            s += QString(" (%1)").arg(Util::formateSongDuration(t.toInt()));
        return s;
    }
    case ModelRoles::RoleTrackImageUri: {
        str = mMusicLibraryManager->querySongImageUri(hash);
        if (str.isEmpty () && mAutoFetchMetadata) {
            mLookupMgr->lookupAlbumImage (hash);
            mLookupMgr->lookupArtistImage (hash);
        }
        return str;
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
        qDebug()<<Q_FUNC_INFO<<" hash list is empty";
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

} //QmlPlugin
} //PhoenixPlayer
