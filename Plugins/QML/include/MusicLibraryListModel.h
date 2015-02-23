#ifndef MUSICLIBRARYLISTMODEL_H
#define MUSICLIBRARYLISTMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "Common.h"

class QByteArray;
class QVariant;
namespace PhoenixPlayer {

namespace MusicLibrary {
class MusicLibraryManager;
}
namespace QmlPlugin {

class MusicLibraryListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MusicLibraryListModel(QObject *parent = 0);
    ~MusicLibraryListModel();

    ///
    /// \brief showAllTracks 显示所有歌曲
    ///
    Q_INVOKABLE void showAllTracks();

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

public slots:
    void clear();

private:
    void appendToModel();
    QString queryOne(const QString &hash,
                     Common::SongMetaTags tag,
                     bool skipDuplicates = true) const;
private:
    QStringList mSongHashList;
    MusicLibrary::MusicLibraryManager *mMusicLibraryManager;
};

} //QmlPlugin
} //PhoenixPlayer
#endif // MUSICLIBRARYLISTMODEL_H
