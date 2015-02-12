#include "MusicLibraryListModel.h"

#include <QMetaEnum>
#include <QByteArray>
#include <QVariant>

#include "MusicLibraryManager.h"
#include "Common.h"

namespace PhoenixPlayer {
using namespace MusicLibrary;

namespace QmlPlugin {

MusicLibraryListModel::MusicLibraryListModel(QObject *parent) :
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
        return queryOne(hash, Common::E_AlbumImageUrl, true);
    case ModelRoles::RoleAlbumName:
        return queryOne(hash, Common::E_AlbumName, true);
    case ModelRoles::RoleArtistImageUri:
        return queryOne(hash, Common::E_ArtistImageUri, true);
    case ModelRoles::RoleArtistName:
        return queryOne(hash, Common::E_ArtistName, true);
    case ModelRoles::RoleCoverArtLarge:
        return queryOne(hash, Common::E_CoverArtLarge, true);
    case ModelRoles::RoleCoverArtMiddle:
        return queryOne(hash, Common::E_CoverArtMiddle, true);
    case ModelRoles::RoleCoverArtSmall:
        return queryOne(hash, Common::E_CoverArtSmall, true);
    case ModelRoles::RoleFileName:
        return queryOne(hash, Common::E_FileName, true);
    case ModelRoles::RoleFilePath:
        return queryOne(hash, Common::E_FilePath, true);
    case ModelRoles::RoleGenre:
        return queryOne(hash, Common::E_Genre, true);
    case ModelRoles::RoleMediaType:
        return queryOne(hash, Common::E_MediaType, true);
    case ModelRoles::RoleSongTitle:
        return queryOne(hash, Common::E_SongTitle, true);
    case ModelRoles::RoleUserRating:
        return queryOne(hash, Common::E_UserRating, true);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MusicLibraryListModel::roleNames() const
{
    QHash<int, QByteArray> role;
    //必须根据enum排列顺序
    for (int i = (int)ModelRoles::RoleFilePath;
         i <= (int)ModelRoles::RoleGenre;
         ++i) {
        role.insert(i, enumToRole(i));
    }
    return role;
}

void MusicLibraryListModel::clear()
{
    for (int i=0; i<mSongHashList.size(); ++i) {
        beginRemoveRows(QModelIndex(), i, i);
        endRemoveRows();
    }
    mSongHashList.clear();
}

QByteArray MusicLibraryListModel::enumToRole(int enumValue) const
{
    int index  = metaObject ()->indexOfEnumerator ("ModelRoles");
    QMetaEnum m = metaObject ()->enumerator (index);
    return QByteArray(m.valueToKey (enumValue)).replace("Role", "").toLower();
}

void MusicLibraryListModel::appendToModel(bool clearBeforeAppend)
{
    if (clearBeforeAppend)
        clear();

    if (mSongHashList.isEmpty())
        return;

    for (int i=0; i<mSongHashList.size (); ++i) {
        beginInsertRows (QModelIndex(), i, i);
        endInsertRows ();
    }
}

QString MusicLibraryListModel::queryOne(const QString &hash,
                                         Common::SongMetaTags tag,
                                         bool skipDuplicates) const
{
    QStringList list = mMusicLibraryManager
            ->querySongMetaElement(tag, hash, skipDuplicates);
    if (list.isEmpty())
        return QString();
    return list.first();
}

} //QmlPlugin
} //PhoenixPlayer
