#ifndef SQLITE3DAO_H
#define SQLITE3DAO_H

#include <QObject>
#include <QHash>
#include <QSqlDatabase>

#include "IPlayListDAO.h"


class QSqlDatabase;

namespace PhoenixPlayer {
namespace PlayList {
namespace SQLite3 {

#define DAO_NAME "SQLite3"
#define VERSION "0.1"

#define DATABASE_NAME ":PhoenixPlayer_sqlite3:"
#define LIBRARY_TAG "LIBRARY"
#define PLAYLIST_TAG "PLAYLIST"

class PhoenixPlayer::SongMetaData;
class SQLite3DAO : public IPlayListDAO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayList.SQLite3DAO" FILE "playlistsqlite3plugin.json")
    Q_INTERFACES(PhoenixPlayer::PlayList::IPlayListDAO)
public:
    SQLite3DAO(QObject *parent = 0);
    static SQLite3DAO *getInstance();
    virtual ~SQLite3DAO();

    // IPlayListDAO interface
public:
    QString getPluginName();
    QString getPluginVersion();
    bool initDataBase();
    bool openDataBase();
//    bool beginTransaction();
//    bool commitTransaction();

//    bool insertMetaData(SongMetaData *metaData);

//    bool updateMetaData(SongMetaData *metaData);
//    bool updateMetaData (const QString &hash, const QString &columnName, const QVariant &newValue);

    bool deleteMetaData(SongMetaData *metaData);
    bool deleteMetaData(const QString &hash);

//    bool execSqlQuery(const QSqlQuery &query);
//    QVariant query(const QString &sql);


//    // IPlayListDAO interface
//public:
//    bool createPlayList(const QString &playListName);
//    bool deletePlayList(const QString &playListName);

    // IPlayListDAO interface
public:
    bool updateMetaData(SongMetaData *metaData, bool sikpEmptyValue);
    QVariant match(Common::MusicLibraryElement e, const QString &condition);
    bool update(Common::MusicLibraryElement targetE, Common::MusicLibraryElement indexE, const QString &indexValue, const QVariant &newVaule);

    // IPlayListDAO interface
public:
    bool beginTransaction();
    bool commitTransaction();
    bool insertMetaData(SongMetaData *metaData);

private:
    QHash<QString, SongMetaData*> mHashList;
    QSqlDatabase mDatabase;
};

} //SQLite3
} //PlayList
} //PhoenixPlayer
#endif // SQLITE3DAO_H
