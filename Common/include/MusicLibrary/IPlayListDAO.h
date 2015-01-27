#ifndef IPLAYLISTDAO_H
#define IPLAYLISTDAO_H

#include <QObject>
#include <QVariant>
#include <QtSql/QSqlQuery>

#include "Common.h"

class QVariant;
class QSqlQuery;

namespace PhoenixPlayer {
class SongMetaData;
namespace MusicLibrary {

class IPlayListDAO : public QObject
{
    Q_OBJECT
public:
    explicit IPlayListDAO(QObject *parent = 0);
    virtual ~IPlayListDAO();

    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;

    virtual bool initDataBase() = 0;

    virtual bool insertMetaData(SongMetaData *metaData = 0, bool skipDuplicates = true) = 0;
    virtual bool updateMetaData(SongMetaData *metaData = 0, bool skipEmptyValue = true) = 0;
    virtual bool deleteMetaData(SongMetaData *metaData = 0) = 0;
    virtual bool deleteMetaData(const QString &hash) = 0;
    virtual SongMetaData *querySongMeta(const QString &hash, const QString &table) = 0;

    virtual QStringList getSongHashList(const QString &playListHash) = 0;

    ///
    /// \brief queryMusicLibrary 搜索音乐列表中targetColumn中的值,条件为regColumn值=regValue
    /// \param targetColumn
    /// \param regColumn
    /// \param regValue
    /// \param skipDuplicates 是否跳过重复的值
    /// \return
    ///
    virtual QStringList queryMusicLibrary(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                                Common::MusicLibraryElement regColumn = Common::E_NULLElement,
                                const QString &regValue = "", bool skipDuplicates = true) = 0;

    ///
    /// \brief queryPlayList 播放列表操作类,
    /// \param targetColum 需要搜索的某一个列
    /// \param regColumn 条件列
    /// \param regValue 条件列的值
    /// \return
    ///
    virtual QStringList queryPlayList(Common::PlayListElement targetColum = Common::E_PlayListNullElement,
                                      Common::PlayListElement regColumn = Common::E_PlayListNullElement,
                                      const QString &regValue = "") = 0;

    ///
    /// \brief updatePlayList 更新播放列表数据
    /// \param targetColumn
    /// \param hash
    /// \param newValue
    /// \param appendNewValues 是否将newValue添加到已有数据后面,否则就从已有数据里面删除newValue,只有当targetColumn = E_PlayListSongHashes才有效
    /// \return
    ///
    //TODO: 怎么const QString后面也需要赋值了,不然编译通不过
    virtual bool updatePlayList(Common::PlayListElement targetColumn = Common::E_PlayListNullElement,
                                const QString &hash = "", const QString &newValue = "", bool appendNewValues = true) = 0;

    virtual bool deletePlayList(const QString &playListHash) = 0;
    virtual bool insertPlayList(const QString &playListName) = 0;

signals:

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

} //MusicLibrary
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::MusicLibrary::IPlayListDAO, "PhoenixPlayer.MusicLibrary.IPlayListDAO/1.0")


#endif // IPLAYLISTDAO_H
