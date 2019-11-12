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

class InnerNode;
class SQLite3DAO : public IMusicLibraryDAO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicLibraryDAO.SQLite3DAO" FILE "playlistsqlite3plugin.json")
    Q_INTERFACES(PhoenixPlayer::BasePlugin)
public:
    explicit SQLite3DAO(QObject *parent = Q_NULLPTR);
    virtual ~SQLite3DAO() Q_DECL_OVERRIDE;
    bool openDataBase();

    // BasePlugin interface
public:
    PluginProperty property() const Q_DECL_OVERRIDE;

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
    bool setLastPlayedTime(const QString &hash, qint64 secs) Q_DECL_OVERRIDE;
    qint64 getLastPlayedTime(const QString &hash) const Q_DECL_OVERRIDE;
    bool setLastPlayedTime(const LastPlayedMeta &meta) Q_DECL_OVERRIDE;
    LastPlayedMeta getLastPlayedMeta(const QString &hash) const Q_DECL_OVERRIDE;
    QList<LastPlayedMeta> getLastPlayedMeta(int limit, bool orderByDesc) const Q_DECL_OVERRIDE;
    QList<LastPlayedMeta> getLastPlayedByAlbum(int limit, bool orderByDesc) const Q_DECL_OVERRIDE;
    QList<LastPlayedMeta> getLastPlayedByArtist(int limit, bool orderByDesc) const Q_DECL_OVERRIDE;
    QList<LastPlayedMeta> getLastPlayedByGenres(int limit, bool orderByDesc) const Q_DECL_OVERRIDE;
    QStringList trackHashListByPlayedCount(bool orderByDesc) const Q_DECL_OVERRIDE;
    QStringList trackHashListByLastPlayedTime(bool orderByDesc) const Q_DECL_OVERRIDE;
    void insertSpectrumData(const AudioMetaObject &obj, const QList<QList<qreal> > &list) Q_DECL_OVERRIDE;
    QList<QList<qreal> > getSpectrumData(const AudioMetaObject &obj) const Q_DECL_OVERRIDE;

    // IMusicLibraryDAO interface
public slots:
    bool beginTransaction() Q_DECL_OVERRIDE;
    bool commitTransaction() Q_DECL_OVERRIDE;

private:
    void calcExistSongs();
    void calcUtilityTable();
    void calcLastPlayedTable();
    bool checkDatabase();
private:
    QSqlDatabase                    m_database;
//    QStringList                     m_existSongHashes;
    bool                            m_transaction;
//    QMap<QString, int>              m_playedCntMap;
//    QMap<QString, InnerNode>        m_playedCntMap;
//    QMap<QString, bool>             m_likeMap;
    QMap<QString, InnerNode>        m_utilityMap;
    QMap<QString, LastPlayedMeta>   m_lastPlayedMap;
    QMap<QString, AudioMetaObject>  m_objMap;
};

} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
#endif // SQLITE3DAO_H
