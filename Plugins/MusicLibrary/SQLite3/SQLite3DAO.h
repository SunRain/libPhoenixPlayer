#ifndef SQLITE3DAO_H
#define SQLITE3DAO_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariant>

#include "MusicLibrary/IPlayListDAO.h"

class QString;
namespace PhoenixPlayer {
class SongMetaData;
class Common;
namespace MusicLibrary {
namespace SQLite3 {

#define DATABASE_NAME "PhoenixPlayer_sqlite3"
#define LIBRARY_TABLE_TAG "LIBRARY"
#define PLAYLIST_TABLE_TAG "PLAYLIST"

class SQLite3DAO : public IPlayListDAO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.PlayListDAO.SQLite3DAO" FILE "playlistsqlite3plugin.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::IPlayListDAO)
public:
    explicit SQLite3DAO(QObject *parent = 0);
//    static SQLite3DAO *getInstance();
    virtual ~SQLite3DAO();

    // IPlayListDAO interface
public:
    bool initDataBase();
    bool openDataBase();

    bool deleteMetaData(SongMetaData *metaData = 0);
    bool deleteMetaData(const QString &hash);
    bool updateMetaData(SongMetaData *metaData = 0, bool skipEmptyValue = true);
    bool insertMetaData(SongMetaData *metaData = 0, bool skipDuplicates = true);
    SongMetaData *querySongMeta(const QString &hash, const QString &table);
    QStringList getSongHashList(const QString &playListHash);

    QStringList queryMusicLibrary(Common::SongMetaTags,
                                  Common::SongMetaTags regColumn,
                                  const QString &regValue, bool skipDuplicates = true);

    QStringList queryPlayList(Common::PlayListElement targetColumn,
                              Common::PlayListElement regColumn,
                              const QString &regValue);

    bool updatePlayList(Common::PlayListElement targetColumn,
                        const QString &hash, const QString &newValue,
                        bool appendNewValues = true);

    bool deletePlayList(const QString &playListHash);
    bool insertPlayList(const QString &playListName);

    // IPlayListDAO interface
public slots:
    bool beginTransaction();
    bool commitTransaction();
private:
    QString listToString(const QStringList &list);
    QStringList stringToList(const QString &str);
    void calcExistSongs();
    ///
    /// \brief fillValues 如果skipEmptyValue为真,则如果value是空值,就返回defaultValue;否则直接返回value
    /// \param value
    /// \param defaultValue
    /// \param skipEmptyValue
    /// \return
    ///
    QString fillValues(const QVariant &value, const QVariant &defaultValue,
                       bool skipEmptyValue = true);
//    int fillValues (int value, int defaultVaule = 0, bool skipEmptyValue = true);

    bool checkDatabase();
private:
    QSqlDatabase mDatabase;
    QStringList mExistSongHashes;
    Common mCommon;
    bool mTransaction;
};

} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
#endif // SQLITE3DAO_H
