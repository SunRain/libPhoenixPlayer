#ifndef SQLITE3DAO_H
#define SQLITE3DAO_H

#include <QObject>
#include <QStringList>
#include <QSqlDatabase>
#include <QVariant>

#include "MusicLibrary/IMusicLibraryDAO.h"

class QString;
namespace PhoenixPlayer {
class AudioMetaObject;
class PPCommon;
namespace MusicLibrary {
namespace SQLite3 {

class SQLite3DAO : public IMusicLibraryDAO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicLibraryDAO.SQLite3DAO" FILE "playlistsqlite3plugin.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::IMusicLibraryDAO)
public:
    explicit SQLite3DAO(QObject *parent = Q_NULLPTR);
    virtual ~SQLite3DAO() Q_DECL_OVERRIDE;
    bool openDataBase();

    // IMusicLibraryDAO interface
public:
    bool initDataBase() Q_DECL_OVERRIDE;
    bool insertMetaData(const AudioMetaObject &obj, bool skipDuplicates) Q_DECL_OVERRIDE;
    bool updateMetaData(const AudioMetaObject &obj, bool skipEmptyValue) Q_DECL_OVERRIDE;
    bool deleteMetaData(const AudioMetaObject &obj) Q_DECL_OVERRIDE;
    bool deleteByHash(const QString &hash) Q_DECL_OVERRIDE;
    AudioMetaObject trackFromHash(const QString &hash) const Q_DECL_OVERRIDE;
    QStringList trackHashList() const Q_DECL_OVERRIDE;
    bool setLike(const QString &hash, bool like) Q_DECL_OVERRIDE;
    bool isLike(const QString &hash) const Q_DECL_OVERRIDE;
    bool setPlayedCount(const QString &hash, int count) Q_DECL_OVERRIDE;
    int playedCount(const QString &hash) const Q_DECL_OVERRIDE;

    // IMusicLibraryDAO interface
public slots:
    bool beginTransaction() Q_DECL_OVERRIDE;
    bool commitTransaction() Q_DECL_OVERRIDE;

private:
    void calcExistSongs();
    void calcUtilityTable();
    bool checkDatabase();
private:
    QSqlDatabase            m_database;
    QStringList             m_existSongHashes;
    bool                    m_transaction;
    QMap<QString, int>      m_playedCntMap;
    QMap<QString, bool>     m_likeMap;
};

} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
#endif // SQLITE3DAO_H
