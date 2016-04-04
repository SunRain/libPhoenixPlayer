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
class Common;
namespace MusicLibrary {
namespace SQLite3 {

//#define PLAYLIST_TABLE_TAG "PLAYLIST"

class SQLite3DAO : public IMusicLibraryDAO
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "PhoenixPlayer.MusicLibraryDAO.SQLite3DAO" FILE "playlistsqlite3plugin.json")
    Q_INTERFACES(PhoenixPlayer::MusicLibrary::IMusicLibraryDAO)
public:
    explicit SQLite3DAO(QObject *parent = 0);
    virtual ~SQLite3DAO();
    bool openDataBase();

    // IMusicLibraryDAO interface
public:
    bool initDataBase();

    bool insertMetaData(const AudioMetaObject &obj, bool skipDuplicates = true);
    bool updateMetaData(const AudioMetaObject &obj, bool skipEmptyValue = true);
//    bool fillAttribute(AudioMetaObject **meta);
    bool deleteMetaData(const AudioMetaObject &obj);
    bool deleteByHash(const QString &hash);
    AudioMetaObject trackFromHash(const QString &hash) const;
    QStringList trackHashList() const;
//    bool deleteMetaData(const QString &hash);
//    bool updateMetaData(SongMetaData *metaData = 0, bool skipEmptyValue = true);
//    bool insertMetaData(SongMetaData *metaData = 0, bool skipDuplicates = true);
//    SongMetaData *querySongMeta(const QString &hash, const QString &table);
//    QStringList getSongHashList(const QString &playListHash);

//    bool fillAttribute(SongMetaData *meta);

//    QStringList queryMusicLibrary(Common::SongMetaTags,
//                                  Common::SongMetaTags regColumn,
//                                  const QString &regValue, bool skipDuplicates = true);

//    QStringList queryPlayList(Common::PlayListElement targetColumn,
//                              Common::PlayListElement regColumn,
//                              const QString &regValue);

//    bool updatePlayList(Common::PlayListElement targetColumn,
//                        const QString &hash, const QString &newValue,
//                        bool appendNewValues = true);

//    bool deletePlayList(const QString &playListHash);
//    bool insertPlayList(const QString &playListName);

    // IMusicLibraryDAO interface
public slots:
    bool beginTransaction();
    bool commitTransaction();
private:
//    template <class T>
//    inline QString classToKey() {
//        QString cls = T::staticMetaObject.className ();
//        if (cls.contains ("::")) {
//            QStringList list = cls.split("::");
//            if (!list.isEmpty ())
//                return list.last ();
//        }
//        return cls;
//    }

    QStringList songMetaDataPropertyList();
//    QString listToString(const QStringList &list);
//    QStringList stringToList(const QString &str);
    void calcExistSongs();
    ///
    /// \brief fillValues 如果skipEmptyValue为真,则如果value是空值,就返回defaultValue;否则直接返回value
    /// \param value
    /// \param defaultValue
    /// \param skipEmptyValue
    /// \return
    ///
//    QString fillValues(const QVariant &value, const QVariant &defaultValue,
//                       bool skipEmptyValue = true);
//    int fillValues (int value, int defaultVaule = 0, bool skipEmptyValue = true);

    bool checkDatabase();
private:
    QSqlDatabase m_database;
    QStringList m_existSongHashes;
//    Common m_common;
    bool m_transaction;
};

} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
#endif // SQLITE3DAO_H
