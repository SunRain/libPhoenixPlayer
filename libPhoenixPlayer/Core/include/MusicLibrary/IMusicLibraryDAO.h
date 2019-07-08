#ifndef IMUSICLIBRARYDAO_H
#define IMUSICLIBRARYDAO_H

#include <QObject>
#include <QVariant>

#include "PPCommon.h"
#include "AudioMetaObject.h"

namespace PhoenixPlayer {
namespace MusicLibrary {

class IMusicLibraryDAO : public QObject
{
    Q_OBJECT
public:
    explicit IMusicLibraryDAO(QObject *parent = 0) : QObject(parent){}
    virtual ~IMusicLibraryDAO() {}

    virtual bool initDataBase() = 0;

    ///
    /// \brief insertMetaData insert track to database
    /// \param metaData the track
    /// \param skipDuplicates skip if track already exists in database
    /// \return
    ///
    virtual bool insertMetaData(const AudioMetaObject &obj, bool skipDuplicates = true) = 0;
    ///
    /// \brief updateMetaData update track meta
    /// \param metaData the track
    /// \param skipEmptyValue use origin value if new track has some empty values
    /// \return
    ///
    virtual bool updateMetaData(const AudioMetaObject &obj, bool skipEmptyValue = true) = 0;
    virtual bool deleteMetaData(const AudioMetaObject &obj) = 0;
    virtual bool deleteByHash(const QString &hash) = 0;
//    virtual SongMetaData *querySongMeta(const QString &hash, const QString &table) = 0;

//    virtual QStringList getSongHashList(const QString &playListHash) = 0;

    ///
    /// \brief fillAttribute 从媒体库中填充属性到目标
    /// \param meta 目标指针
    /// \return true如果填充成功
    ///
//    virtual bool fillAttribute(AudioMetaObject **meta) = 0;

//    ///
//    /// \brief allTracks
//    /// \return 数据库中的所有曲目
//    ///
//    virtual QList<SongMetaData *> allTracks() = 0;

    virtual AudioMetaObject trackFromHash(const QString &hash) const = 0;

    virtual QStringList trackHashList() const = 0;

    virtual QStringList trackHashListByPlayedCount(bool orderByDesc = true) const = 0;

    virtual QStringList trackHashListByLastPlayedTime(bool orderByDesc = true) const = 0;

    virtual bool setLike(const QString &hash, bool like) = 0;

    virtual bool isLike(const QString &hash) const = 0;

    ///
    /// \brief setPlayedCount set count of a track played
    /// \param hash
    /// \param count
    /// \return
    ///
    virtual bool setPlayedCount(const QString &hash, int count) = 0;

    virtual int playedCount(const QString &hash) const = 0;

    virtual bool setLastPlayedTime(const QString &hash, qint64 secs) = 0;

    virtual bool setLastPlayedTime(const LastPlayedMeta &meta) = 0;

    virtual qint64 getLastPlayedTime(const QString &hash) const = 0;

    virtual LastPlayedMeta getLastPlayedMeta(const QString &hash) const = 0;

    ///
    /// \brief getLastPlayedMeta
    /// \param limit limit size
    /// \param orderByDesc
    /// TRUE order by timestamp with desc
    /// FALSE order by timestamp with asc
    /// \return
    ///
    virtual QList<LastPlayedMeta> getLastPlayedMeta(int limit = 20, bool orderByDesc = true) const = 0;

    ///
    /// \brief getLastPlayedByAlbum
    /// \param limit the limit of album size
    /// \param orderByDesc
    /// TRUE order by timestamp with desc
    /// FALSE order by timestamp with asc
    /// \return
    ///
    virtual QList<LastPlayedMeta> getLastPlayedByAlbum(int limit = 20, bool orderByDesc = true) const = 0;

    virtual QList<LastPlayedMeta> getLastPlayedByArtist(int limit = 20, bool orderByDesc = true) const = 0;

    virtual QList<LastPlayedMeta> getLastPlayedByGenres(int limit = 20, bool orderByDesc = true) const = 0;

    virtual void insertSpectrumData(const AudioMetaObject &obj, const QList<QList<qreal>> &list) = 0;

    virtual QList<QList<qreal>> getSpectrumData(const AudioMetaObject &obj) const = 0;
//    ///
//    /// \brief queryMusicLibrary 搜索音乐列表中targetColumn中的值,条件为regColumn值=regValue
//    /// \param targetColumn
//    /// \param regColumn
//    /// \param regValue
//    /// \param skipDuplicates 是否跳过重复的值
//    /// \return
//    ///
//    virtual QStringList queryMusicLibrary(Common::SongMetaTags targetColumn,
//                                Common::SongMetaTags regColumn,
//                                const QString &regValue, bool skipDuplicates = true) = 0;

//    ///
//    /// \brief queryPlayList 播放列表操作类,
//    /// \param targetColum 需要搜索的某一个列
//    /// \param regColumn 条件列
//    /// \param regValue 条件列的值
//    /// \return
//    ///
//    virtual QStringList queryPlayList(Common::PlayListElement targetColum,
//                                      Common::PlayListElement regColumn,
//                                      const QString &regValue) = 0;

//    ///
//    /// \brief updatePlayList 更新播放列表数据
//    /// \param targetColumn
//    /// \param hash
//    /// \param newValue
//    /// \param appendNewValues 是否将newValue添加到已有数据后面,
//    /// 否则就从已有数据里面删除newValue,只有当targetColumn = E_PlayListSongHashes才有效
//    /// \return
//    ///
//    virtual bool updatePlayList(Common::PlayListElement targetColumn,
//                                const QString &hash, const QString &newValue ,
//                                bool appendNewValues = true) = 0;

//    virtual bool deletePlayList(const QString &playListHash) = 0;
//    virtual bool insertPlayList(const QString &playListName) = 0;

signals:
    void metaDataInserted(const QString &hash);
    void metaDataDeleted(const QString &hash);
//    void libraryChanged();

public slots:
    ///
    /// \brief beginTransaction 开始事务
    /// \return
    ///
    virtual bool beginTransaction() = 0;

    ///
    /// \brief commitTransaction 提交/结束事务
    /// \return
    ///
    virtual bool commitTransaction() = 0;
};

} //IMUSICLIBRARYDAO_H
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::MusicLibrary::IMusicLibraryDAO, "PhoenixPlayer.MusicLibraryDAO.IMusicLibraryDAO/1.0")


#endif // IPLAYLISTDAO_H
