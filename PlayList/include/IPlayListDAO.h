#ifndef IPLAYLISTDAO_H
#define IPLAYLISTDAO_H

#include <QObject>
#include "SongMetaData.h"

namespace PhoenixPlayer {
namespace PlayList {
class SongMetaData;
class IPlayListDAO : public QObject
{
    Q_OBJECT
public:
    explicit IPlayListDAO(QObject *parent = 0);
    virtual ~IPlayListDAO();

    virtual bool initDataBase() = 0;
    virtual bool openDataBase() = 0;
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
    virtual bool updateMetaData (const QString &hash, SongMetaData *data = 0) = 0;

signals:

public slots:


};

} //PlayList
} //PhoenixPlayer

#endif // IPLAYLISTDAO_H
