#ifndef IPLAYLISTDAO_H
#define IPLAYLISTDAO_H

#include <QObject>
#include <QVariant>
#include <QtSql/QSqlQuery>

#include "SongMetaData.h"

class QVariant;
class QSqlQuery;

namespace PhoenixPlayer {
namespace PlayList {

class SongMetaData;
class IPlayListDAO : public QObject
{
    Q_OBJECT
public:
    explicit IPlayListDAO(QObject *parent = 0);
    virtual ~IPlayListDAO();

    virtual QString getPluginName() = 0;
    virtual QString getPluginVersion() = 0;

    virtual bool initDataBase() = 0;
    virtual bool createPlayList(const QString &playListName) = 0;
    virtual bool deletePlayList(const QString &playListName) = 0;

//    virtual bool openDataBase() = 0;
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

    virtual bool insertMetaData(SongMetaData *metaData = 0) = 0;
    virtual bool updateMetaData(SongMetaData *metaData = 0) = 0;
//    virtual bool updateMetaData (const QString &hash, SongMetaData *data = 0) = 0;
    virtual bool updateMetaData (const QString &hash, const QString &columnName, const QVariant &newValue) = 0;
    virtual bool deleteMetaData(SongMetaData *metaData = 0) = 0;
    virtual bool deleteMetaData(const QString &hash) = 0;

    virtual bool execSqlQuery(const QSqlQuery &query) = 0;
//    QVariant query (const QString &columnName, const QVariant &value) = 0;
    virtual QVariant query(const QString &sql) = 0;

signals:

public slots:


};

} //PlayList
} //PhoenixPlayer

Q_DECLARE_INTERFACE(PhoenixPlayer::PlayList::IPlayListDAO, "PhoenixPlayer.PlayList.IPlayListDAO/1.0")


#endif // IPLAYLISTDAO_H
