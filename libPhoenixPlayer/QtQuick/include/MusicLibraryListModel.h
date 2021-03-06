#ifndef MUSICLIBRARYLISTMODEL_H
#define MUSICLIBRARYLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "Common.h"

class QByteArray;
class QVariant;
namespace PhoenixPlayer {
class PlayerCore;

namespace MusicLibrary {
class MusicLibraryManager;
}
namespace MetadataLookup {
class MetadataLookupMgrWrapper;
}

namespace QmlPlugin {

class MusicLibraryListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool autoFetchMetadata READ autoFetchMetadata WRITE setAutoFetchMetadata NOTIFY autoFetchMetadataChanged)
public:
    explicit MusicLibraryListModel(QAbstractListModel *parent = 0);
    ~MusicLibraryListModel();

    ///
    /// \brief showAllTracks 显示所有歌曲
    ///
    Q_INVOKABLE void showAllTracks();

    Q_INVOKABLE void showArtistTracks(const QString &artistName,int limitNum = -1);
    Q_INVOKABLE void showAlbumTracks(const QString  &albumName, int limitNum = -1);
    Q_INVOKABLE void showGenreTracks(const QString &genreName, int limitNum = -1);
    Q_INVOKABLE void showMediaTypeTracks(const QString &mediaType, int limitNum = -1);
    Q_INVOKABLE void showUserRatingTracks(const QString &rating, int limitNum = -1);
    Q_INVOKABLE void showFolderTracks(const QString &folder, int limitNum = -1);
    Q_INVOKABLE void showPlayListItems(const QString &playListHash, int limitNum = -1);

    void setAutoFetchMetadata(bool autoFetch);
    bool autoFetchMetadata();

    Q_INVOKABLE void fetchTrackImage(const QString &hash);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
public:
    enum ModelRoles {
        RoleFilePath = Qt::UserRole + 1,
        RoleFileName,
        RoleArtistName,
        RoleArtistImageUri,
        RoleAlbumName,
        RoleAlbumImageUrl,
        RoleCoverArtSmall,
        RoleCoverArtLarge,
        RoleCoverArtMiddle,
        RoleMediaType,
        RoleSongTitle,
        RoleUserRating,
        RoleGenre,
        RoleHash,
        RoleTrackTitle,     //播放列表中第一行显示内容
        RoleTrackSubTitle,   //播放列表中第二行显示内容
        /*
         * 歌曲封面图片，按照
         * CoverArtMiddle
         * CoverArtLarge
         * CoverArtSmall
         * AlbumImageUrl
         * ArtistImageUri
         * 顺序返回图片地址，如果都不存在，则返回空
         */
        RoleTrackImageUri
    };

signals:
    void autoFetchMetadataChanged();
    void trackImageFetched(const QString &hash);
public slots:
    void clear();

private:
    void appendToModel(int limitNum = -1);
//    QString queryOne(const QString &hash,
//                     Common::SongMetaTags tag,
//                     bool skipDuplicates = true) const;
private:
    QStringList mSongHashList;
    MusicLibrary::MusicLibraryManager *mMusicLibraryManager;
//    Player *mPlayer;
    MetadataLookup::MetadataLookupMgrWrapper *mLookupMgr;
    int mLimitNum;
    bool mAutoFetchMetadata;
};

} //QmlPlugin
} //PhoenixPlayer
#endif // MUSICLIBRARYLISTMODEL_H
