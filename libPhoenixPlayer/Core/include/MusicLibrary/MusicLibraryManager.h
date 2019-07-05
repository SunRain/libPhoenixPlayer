#ifndef MUSICLIBRARYMANAGER_H
#define MUSICLIBRARYMANAGER_H

#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QList>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"
#include "SingletonPointer.h"
#include "BaseObject.h"
#include "AudioMetaObject.h"
#include "AudioMetaGroupObject.h"

namespace PhoenixPlayer {
class PPSettings;
class PluginLoader;
class LibPhoenixPlayer;

namespace MusicLibrary {

class MusicLibraryDAOHost;
class IMusicLibraryDAO;
class LocalMusicScanner;

class LIBPHOENIXPLAYER_EXPORT MusicLibraryManager : public BaseObject
{
    Q_OBJECT
    friend class PhoenixPlayer::LibPhoenixPlayer;
protected:
    explicit MusicLibraryManager(PPSettings *set, PluginLoader *loader, QObject *parent = Q_NULLPTR);
    virtual ~MusicLibraryManager();
public:

    LocalMusicScanner *localMusicScanner() const;

    ///
    /// \brief allTracks 显示所有曲目
    /// \return 空列表如果没有曲目
    ///
    AudioMetaList allTracks();

    AudioMetaObject trackFromHash(const QString &hash) const;

    ///
    /// \brief empty wether current library is empty
    /// \return true if empty
    ///
    Q_INVOKABLE bool empty() const;

    Q_INVOKABLE inline bool isEmpty() const {return empty();}

    void deleteObject(const AudioMetaObject &obj, bool deleteFromLocalDisk = false);

    void deleteObject(const QString &hash, bool deleteFromLocalDisk = false);

    ///
    /// \brief artistTracks 显示某个artist下的曲目
    /// \param artistName artist名
    /// \param limitNum 显示个数
    /// \return 空列表如果没有曲目
    ///
    AudioMetaList artistTracks(const QString &artistName,int limitNum = 0);
    AudioMetaList albumTracks(const QString  &albumName, int limitNum = 0);
    AudioMetaList genreTracks(const QString &genreName, int limitNum = 0);
    AudioMetaList mediaTypeTracks(const QString &mediaType, int limitNum = 0);
    AudioMetaList userRatingTracks(const QString &rating, int limitNum = 0);
    AudioMetaList folderTracks(const QString &folder, int limitNum = 0);

    AudioMetaGroupList artistList() const;
    AudioMetaGroupList albumList() const;
    AudioMetaGroupList genreList() const;

    void setLike(const QString &hash, bool like);
    void setLike(const AudioMetaObject &obj, bool like);

    bool isLike(const QString &hash);
    bool isLike(const AudioMetaObject &obj);

    ///
    /// \brief addLastPlayedTime add last played time for current time
    /// \param hash
    ///
    void addLastPlayedTime(const QString &hash);

    void addLastPlayedTime(const AudioMetaObject &obj);

    qint64 getLastPlayedTime(const QString &hash);

    qint64 getLastPlayedTime(const AudioMetaObject &obj);

    ///
    /// \brief setPlayedCount set count of a track played
    /// \param hash
    /// \param count
    /// \return
    ///
    void setPlayedCount(const QString &hash, int count);
    void setPlayedCount(const AudioMetaObject &obj, int count);
    void addPlayedCount(const QString &hash);
    void addPlayedCount(const AudioMetaObject &obj);

    int playedCount(const QString &hash) const;

    int playedCount(const AudioMetaObject &obj) const;


    LastPlayedMeta getLastPlayedMeta(const QString &hash) const;

    ///
    /// \brief getLastPlayedMeta
    /// \param limit limit size
    /// \param orderByDesc
    /// TRUE order by timestamp with desc
    /// FALSE order by timestamp with asc
    /// \return
    ///
    QList<LastPlayedMeta> getLastPlayedMeta(int limit = 20, bool orderByDesc = true) const;

    ///
    /// \brief getLastPlayedByAlbum
    /// \param limit the limit of album size
    /// \param orderByDesc
    /// TRUE order by timestamp with desc
    /// FALSE order by timestamp with asc
    /// \return
    ///
    QList<LastPlayedMeta> getLastPlayedByAlbum(int limit = 20, bool orderByDesc = true) const;

    QList<LastPlayedMeta> getLastPlayedByArtist(int limit = 20, bool orderByDesc = true) const;

    QList<LastPlayedMeta> getLastPlayedByGenres(int limit = 20, bool orderByDesc = true) const;

    QStringList trackHashListByPlayedCount(bool orderByDesc = true) const;

    QStringList trackHashListByLastPlayedTime(bool orderByDesc = true) const;


    void saveToDB();

    ///
    /// \brief getSpectrumFile
    /// \param obj
    /// \return full path of spectrum data file. Empty if AudioMetaObject is empty
    ///
    static QString getSpectrumFile(const AudioMetaObject &obj);

    ///
    /// \brief loadSpectrumData
    /// \param obj
    /// \return  empty if no spectrum data file found
///
    static QList<QList<qreal> > loadSpectrumData(const AudioMetaObject &obj);


//    ///
//    /// \brief playingSong 返回当前播放的歌曲hash,
//    /// 如果当前未播放,则返回数据库中的第一首歌曲,或者播放器上次播放的歌曲
//    /// \return
//    ///
//    Q_INVOKABLE QString playingSongHash();

//    ///
//    /// \brief setPlayingSongHash 设置需要播放的歌曲在曲库中的hash值
//    /// 当设置的hash值在曲库中时会发送 playingSongChanged 信号
//    /// \param newHash
//    ///
//    Q_INVOKABLE void setPlayingSongHash(const QString &newHash);

//    QString firstSongHash();
//    QString lastSongHash();

//    ///
//    /// \brief nextSong 返回当前播放列表的下一首歌曲,如果在末尾则返回第一首歌
//    /// \param jumpToNextSong 是否同时自动跳转到nextSong,这会发送 playingSongChanged 信号
//    /// \return 当前播放列表的下一首歌曲
//    ///
//    Q_INVOKABLE QString nextSong(bool jumpToNextSong = true);
//    ///
//    /// \brief preSong 当前播放列表的上一首歌曲,如果在第一首,则返回最后一首
//    ///
//    Q_INVOKABLE QString preSong(bool jumpToPreSong = true);
//    ///
//    /// \brief randomSong 转到随机的一首歌曲
//    ///
//    Q_INVOKABLE QString randomSong(bool jumpToRandomSong = true);

//    ///
//    /// \brief querySongTitle 按照SongTitle=>FileName的顺序返回歌曲信息
//    /// \param hash
//    /// \return 如果不存在，则返回空
//    ///
//    Q_INVOKABLE QString querySongTitle(const QString &hash);

//    ///
//    /// \brief querySongImage 歌曲封面图片，
//    /// 按照
//    /// CoverArtMiddle
//    /// CoverArtLarge
//    /// CoverArtSmall
//    /// AlbumImageUrl
//    /// ArtistImageUri
//    /// 顺序返回图片地址，如果都不存在，则返回空
//    /// \param hash
//    /// \return
//    ///
//    Q_INVOKABLE QString querySongImageUri(const QString &hash);

//    QStringList queryMusicLibrary(Common::SongMetaTags targetColumn,
//                                    Common::SongMetaTags regColumn,
//                                    const QString &regValue, bool skipDuplicates = true);

////    Q_INVOKABLE QStringList queryMusicLibraryByIndex(int targetColumn, int regColumn,
////                                                     const QString &regValue, bool skipDuplicates = true);
//    ///
//    /// \brief querySongMetaElement 查询某一个列
//    /// \param targetColumn 需要查询的列
//    /// \param hash hash值
//    /// \param skipDuplicates 是否跳过重复值
//    /// \return
//    ///
//    QStringList querySongMetaElement(Common::SongMetaTags targetColumn,
//                                     const QString &hash,
//                                     bool skipDuplicates = true);

//    ///
//    /// \brief querySongMetaElement 由于qml不支持直接和cpp代码传递enum值,所以用int代替
//    /// \param columnIndex
//    /// \param hash
//    /// \param skipDuplicates
//    /// \return
//    ///
//    Q_INVOKABLE QStringList querySongMetaElementByIndex(int columnIndex,
//                                const QString &hash, bool skipDuplicates = true);


//    ///
//    /// \brief changePlayList 更改播放列表
//    /// \param playListHash
//    /// \return
//    ///
//    Q_INVOKABLE bool changePlayList(const QString &playListHash);
//    Q_INVOKABLE bool createPlayList(const QString &playListName);
//    Q_INVOKABLE bool deletePlayList(const QString &playListHash);
//    Q_INVOKABLE bool insertToPlayList(const QString &playListHash,
//                                      const QString &newSongHash);
//    Q_INVOKABLE bool deleteFromPlayList(const QString &playListHash,
//                                        const QString &songHash,
//                                        bool deleteFromStorage = false);
//    ///
//    /// \brief getCurrentPlayListHash 返回当前播放列表的hash值
//    /// \return 不存在则返回空
//    ///
//    Q_INVOKABLE QString getCurrentPlayListHash();

//    ///
//    /// \brief queryPlayListElement 搜索播放列表相关内容, 如果hash值为空
//    /// \param targetColumn 需要搜索的列
//    /// \param hash 条件hash值
//    /// \return
//    ///
//    QStringList queryPlayListElement(Common::PlayListElement targetColumn,
//                                     const QString &hash);

//    ///
//    /// \brief queryPlayListElementByIndex 由于qml不支持直接和cpp代码传递enum值,所以用int代替
//    /// \param index 需要搜索的列
//    /// \param hash 条件hash值
//    /// \return
//    ///
//    Q_INVOKABLE QStringList queryPlayListElementByIndex(int index,
//                                                        const QString &hash );

//    QString queryOne(const QString &hash, Common::SongMetaTags tag, bool skipDuplicates = true);
//    Q_INVOKABLE QString queryOneByIndex(const QString &hash, int tag, bool skipDuplicates = true);

////protected:
////    explicit MusicLibraryManager(QObject *parent = 0);
signals:
    ///
    /// \brief libraryListSizeChanged
    /// emited when audio meta object removed from library or added to library
    void libraryListSizeChanged();

//    void playingSongChanged();
//    void playListChanged();
//    void playListCreated();
//    void playListDeleted();
//    void playListTrackChanged();
private:
//      bool init();
    void initList();
    void insertToAlbumGroupMap(const AudioMetaObject &obj);
    void insertToArtistGroupMap(const AudioMetaObject &obj);
    void insertToGenreGroupMap(const AudioMetaObject &obj);
//    bool trackInList(AudioMetaObject **data);
private:
    PPSettings                      *m_settings;
    PluginLoader                    *m_pluginLoader;
    MusicLibraryDAOHost             *m_daoHost;
    IMusicLibraryDAO                *m_dao;
    LocalMusicScanner               *m_localMusicScanner;
    AudioMetaList                   m_trackList;
    QMap<QString, bool>             m_likeMap;
    QMap<QString, int>              m_playCntMap;
    QMap<QString, LastPlayedMeta>   m_lastPlayedMap;
    QMap<QString, AudioMetaGroupObject> m_albumGroupMap;
    QMap<QString, AudioMetaGroupObject> m_artistGroupMap;
    QMap<QString, AudioMetaGroupObject> m_genreGroupMap;
};

} //MusicLibrary
} //PhoenixPlayer

#endif // MUSICLIBRARYMANAGER_H
