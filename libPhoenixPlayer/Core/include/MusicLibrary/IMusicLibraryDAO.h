#ifndef IMUSICLIBRARYDAO_H
#define IMUSICLIBRARYDAO_H

#include <QObject>
#include <QVariant>

#include "Common.h"

namespace PhoenixPlayer {
class SongMetaData;
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
    virtual bool insertMetaData(SongMetaData **metaData, bool skipDuplicates = true) = 0;
    ///
    /// \brief updateMetaData update track meta
    /// \param metaData the track
    /// \param skipEmptyValue use origin value if new track has some empty values
    /// \return
    ///
    virtual bool updateMetaData(SongMetaData **metaData, bool skipEmptyValue = true) = 0;
    virtual bool deleteMetaData(SongMetaData **metaData) = 0;
    virtual bool deleteByHash(const QString &hash) = 0;
//    virtual SongMetaData *querySongMeta(const QString &hash, const QString &table) = 0;

//    virtual QStringList getSongHashList(const QString &playListHash) = 0;

    ///
    /// \brief fillAttribute 从媒体库中填充属性到目标
    /// \param meta 目标指针
    /// \return true如果填充成功
    ///
    virtual bool fillAttribute(SongMetaData **meta) = 0;

//    ///
//    /// \brief allTracks
//    /// \return 数据库中的所有曲目
//    ///
//    virtual QList<SongMetaData *> allTracks() = 0;

    virtual SongMetaData *trackFromHash(const QString &hash) = 0;

    virtual QStringList trackHashList() const = 0;

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
    void metaDataInserted();
    void metaDataDeleted();
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
