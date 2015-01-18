#ifndef IPLAYLISTDAO_H
#define IPLAYLISTDAO_H

#include <QObject>
#include <QVariant>
#include <QtSql/QSqlQuery>

#include "SongMetaData.h"
#include "Common.h"

class QVariant;
class QSqlQuery;

namespace PhoenixPlayer {
namespace PlayList {

class IPlayListDAO : public QObject
{
    Q_OBJECT
public:
    explicit IPlayListDAO(QObject *parent = 0);
    virtual ~IPlayListDAO();

    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;

    virtual bool initDataBase() = 0;

    virtual bool insertMetaData(PhoenixPlayer::SongMetaData *metaData = 0, bool skipDuplicates = true) = 0;
    virtual bool updateMetaData(PhoenixPlayer::SongMetaData *metaData = 0, bool skipEmptyValue = true) = 0;
    virtual bool deleteMetaData(PhoenixPlayer::SongMetaData *metaData = 0) = 0;
    virtual bool deleteMetaData(const QString &hash) = 0;
    virtual PhoenixPlayer::SongMetaData *query(const QString &hash, const QString &table) = 0;

    virtual QStringList getSongHashList() = 0;

    ///
    /// \brief queryColumn 搜索音乐列表中targetColumn中的值,条件为regColumn值=regValue
    /// \param targetColumn
    /// \param regColumn
    /// \param regValue
    /// \param skipDuplicates 是否跳过重复的值
    /// \return
    ///
    virtual QStringList queryColumn(Common::MusicLibraryElement targetColumn = Common::E_NULLElement,
                                Common::MusicLibraryElement regColumn = Common::E_NULLElement,
                                const QString &regValue = "", bool skipDuplicates = true) = 0;
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

} //PlayList
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::PlayList::IPlayListDAO, "PhoenixPlayer.PlayList.IPlayListDAO/1.0")


#endif // IPLAYLISTDAO_H
