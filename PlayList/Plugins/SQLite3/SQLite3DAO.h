#ifndef SQLITE3DAO_H
#define SQLITE3DAO_H

#include <QObject>
#include "IPlayListDAO.h"

namespace PhoenixPlayer {
namespace PlayList {
namespace SQLite3 {

#define DAO_NAME "SQLite3"
#define VERSION "0.1"

class SQLite3DAO : public IPlayListDAO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayList.SQLite3DAO" FILE "playlistsqlite3plugin.json")
    Q_INTERFACES(PhoenixPlayer::PlayList::IPlayListDAO)
public:
    SQLite3DAO(QObject *parent = 0);
    virtual ~SQLite3DAO();

    // IPlayListDAO interface
public:
    QString getPluginName();
    QString getPluginVersion();
    bool initDataBase();
    bool openDataBase();
    bool beginTransaction();
    bool commitTransaction();
    bool insertMetaData(SongMetaData *metaData);

    // IPlayListDAO interface
public:
    bool updateMetaData(SongMetaData *metaData);
    bool deleteMetaData(SongMetaData *metaData);
};

} //SQLite3
} //PlayList
} //PhoenixPlayer
#endif // SQLITE3DAO_H
