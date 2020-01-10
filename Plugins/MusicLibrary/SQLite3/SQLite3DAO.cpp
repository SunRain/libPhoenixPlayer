#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSqlQuery>
#include <QJsonObject>

#include <QLatin1String>
#include <QtAlgorithms>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QDataStream>
#include <QByteArray>

#include <QDebug>

#include "SQLite3DAO.h"
#include "AudioMetaObject.h"
#include "PPUtility.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
namespace SQLite3 {

const static char *DATABASE_NAME        = "PhoenixPlayer_musiclibrary";
const static char *TABLE_LIBRARY_TAG    = "LIBRARY";
const static char *TABLE_UTILITY_TAG    = "UTILITY";
const static char *TABLE_LAST_PLAYED    = "LAST_PLAYED";
const static char *TABLE_SPECTRUM       = "SPECTRUM";
const static char *UTILITY_KEY_LIKE     = "LIKE";
const static char *UTILITY_KEY_CNT      = "CNT";
const static char *LP_KEY_HASH          = "HASH";
const static char *LP_KEY_ALBUM_NAME    = "ALBUM_NAME";
const static char *LP_KEY_ARTIST_NAME   = "ARTIST_NAME";
const static char *LP_KEY_GENRES        = "GENRES";
const static char *LP_KEY_TIMESTAMP     = "TIME_STAMP";
const static char *SP_KEY_HASH          = "HASH";
const static char *SP_KEY_DATA          = "SPEK_DATA";

bool sortLastPlayedMetaLessThan(const LastPlayedMeta &a, const LastPlayedMeta &b)
{
    return a.timestamp() < b.timestamp();
}

bool sortLastPlayedMetaGreaterThan(const LastPlayedMeta &a, const LastPlayedMeta &b)
{
    return a.timestamp() > b.timestamp();
}

class InnerNode
{
public:
    InnerNode() : d(new Priv()) {}
    InnerNode(const InnerNode &other) : d(other.d){}

    class Priv : public QSharedData
    {
    public:
        Priv() {}
        int cnt = 0;
        bool like = false;
        QString hash = QString();
    };
    QSharedDataPointer<Priv> d;
    inline InnerNode &operator =(const InnerNode &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    inline bool operator == (const InnerNode &other) {
        return other.d.data()->hash == d.data()->hash &&
                other.d.data()->cnt == d.data()->cnt &&
                other.d.data()->like == d.data()->like;
    }
    inline bool operator != (const InnerNode &other) {
        return !operator == (other);
    }
};

SQLite3DAO::SQLite3DAO(QObject *parent)
    :IMusicLibraryDAO(parent)
{
    m_transaction = false;

    initDataBase ();
}

SQLite3DAO::~SQLite3DAO()
{
    qDebug()<<"--------------";

    if (m_database.isOpen ()) {
        m_database.close ();
    }
    qDebug()<<"------------ after ";
}

bool SQLite3DAO::initDataBase()
{
    qDebug()<<">>>>>>>>>>>>>> ------------ <<<<<<<<<<<<<<<<<<<<<<";

    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString dbFile = QString("%1/%2").arg(dbPath).arg(DATABASE_NAME);

    qDebug()<<"===== path is "<<dbPath;

    QDir dir(dbPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dbPath)) {
            qDebug()<<Q_FUNC_INFO<<" Can't mkdir ";
            return false;
        }
    }

    if (!m_database.isValid ()) {
        qDebug()<<"Database is not valid";
        m_database = QSqlDatabase::database();
        qDebug()<<"Try to add database form connection  'MusicLibrary', ret is "<<m_database.lastError().text();
    }

    if (!m_database.isValid()) {
        qDebug()<<"Add database from connection 'MusicLibrary' failed";
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName(dbFile);
        qDebug()<<"Try to add a new connection MusicLibrary, ret is "<<m_database.lastError().text();
    }

    if (!m_database.isValid()) {
        qDebug()<<"OOPS, We can't connetc to sqlite database "<<m_database.lastError().text();
        m_database.removeDatabase (dbFile);
    }
    if (!m_database.open()) {
        qDebug()<<"Can't open mDatabase "<<m_database.lastError().text();
        m_database.removeDatabase(dbFile);
        return false;
    }

    //    NULL	NULL value.	NULL
    //    INTEGER	Signed integer, stored in 8, 16, 24, 32, 48, or 64-bits depending on the magnitude of the value.	typedef qint8/16/32/64
    //    REAL	64-bit floating point value.	By default mapping to QString
    //    TEXT	Character string (UTF-8, UTF-16BE or UTF-16-LE).	Mapped to QString
    //    CLOB	Character large string object	Mapped to QString
    //    BLOB	The value is a BLOB of data, stored exactly as it was input.	Mapped to QByteArray

    /*
     * 检测数据表是否存在
     */
    QStringList tables = m_database.tables();
    if (tables.contains(TABLE_LIBRARY_TAG, Qt::CaseInsensitive)) {
        qDebug()<<"Found library table now, we will check exist Song Hashes!!!";
        calcExistSongs();
    } else {
        QSqlQuery q;
        QString str;
        str = "create table ";
        str += TABLE_LIBRARY_TAG;
        str += "(";
        str += "id integer primary key, ";

        QJsonObject obj = AudioMetaObject().toObject ();
        foreach (const QString &key, obj.keys ()) {
            str += QString("%1 TEXT, ").arg (key);
        }
        str = str.simplified ();
        str = str.left (str.length () - 1);
        str += ")";


        qDebug()<<"run sql "<<str;
        /*
         * 如果数据表创建出现问题,直接删除整个数据库,防止和后面的检测冲突
         */
        if (!q.exec (str)) {
            qDebug()<<QString("Create library tab error [ %1 ]").arg (q.lastError ().text ());
            m_database.removeDatabase (DATABASE_NAME);
            return false;
        }
    }
    if (tables.contains(TABLE_UTILITY_TAG, Qt::CaseInsensitive)) {
        qDebug()<<"Found utility now, we will check exist data!!!";
        calcUtilityTable();
    } else {
        QSqlQuery q;
        QString str = QString("create table %1 (id integer primary key, %2 TEXT, %3 INTEGER, %4 INTEGER)")
                .arg(TABLE_UTILITY_TAG)
                .arg(AudioMetaObject::Object_Internal_Key_Name_Hash())
                .arg(UTILITY_KEY_CNT)
                .arg(UTILITY_KEY_LIKE);

        qDebug()<<"run sql "<<str;
        /*
         * 如果数据表创建出现问题,直接删除整个数据库,防止和后面的检测冲突
         */
        if (!q.exec (str)) {
            qDebug()<<QString("Create utility tab error [ %1 ]").arg (q.lastError ().text ());
            m_database.removeDatabase (DATABASE_NAME);
            return false;
        }
    }
    if (tables.contains(TABLE_LAST_PLAYED, Qt::CaseInsensitive)) {
        qDebug()<<"Found lastplayed now, we will check exist data!!!";
        calcLastPlayedTable();
    } else {
        QSqlQuery q;
        QString str = QString("create table %1").arg(TABLE_LAST_PLAYED);
        str += " ( ";
        str += "id integer primary key, ";
        str += QString("%1 TEXT, ").arg(LP_KEY_HASH);
        str += QString("%1 TEXT, ").arg(LP_KEY_ALBUM_NAME);
        str += QString("%1 TEXT, ").arg(LP_KEY_ARTIST_NAME);
        str += QString("%1 TEXT, ").arg(LP_KEY_GENRES);
        str += QString("%1 INTEGER").arg(LP_KEY_TIMESTAMP);
        str += ")";

        qDebug()<<"run sql "<<str;
        /*
         * 如果数据表创建出现问题,直接删除整个数据库,防止和后面的检测冲突
         */
        if (!q.exec (str)) {
            qDebug()<<QString("Create utility tab error [ %1 ]").arg (q.lastError ().text ());
            m_database.removeDatabase (DATABASE_NAME);
            return false;
        }
    }
    if (!tables.contains(TABLE_SPECTRUM, Qt::CaseInsensitive)) {
        QSqlQuery q;
        QString str = QString("create table %1 (id integer primary key, %2 TEXT, %3 BLOB)")
                .arg(TABLE_SPECTRUM)
                .arg(SP_KEY_HASH)
                .arg(SP_KEY_DATA);
        qDebug()<<"run sql "<<str;
        /*
         * 如果数据表创建出现问题,直接删除整个数据库,防止和后面的检测冲突
         */
        if (!q.exec (str)) {
            qDebug()<<QString("Create spectrum tab error [ %1 ]").arg(q.lastError().text());
            m_database.removeDatabase(DATABASE_NAME);
            return false;
        }
    }
    return true;
}

bool SQLite3DAO::doInsertMetaData(const AudioMetaObject &obj, bool skipDuplicates)
{
    qDebug()<<"===================";
    if (obj.isHashEmpty()) {
        qDebug()<<"AudioMetaObject empty!!";
        return false;
    }
    if (!checkDatabase ())
        return false;

    if (skipDuplicates) {
        if (m_objMap.contains(obj.hash())) {
            qDebug()<<"skipDuplicates "<<obj.uri ();
            return true;
        }
    }
    QString column, value;
    QJsonObject json = obj.toObject ();
    QStringList keyList = json.keys ();
    foreach (const QString &key, keyList) {
        column += QString("%1, ").arg (key);
        value += QString(":%1, ").arg (key);//QString("\"%1\", ").arg (json.value (key).toString ());
    }
    column = column.simplified ();
    column = column.left (column.length () - 1);
    value = value.simplified ();
    value = value.left (value.length () - 1);

    QString str = "insert into ";
    str += TABLE_LIBRARY_TAG;
    str += " (";
    str += column;
    str += ") values (";
    str += value;
    str += ") ";

    QSqlQuery q;
    if (!q.prepare (str)) {
        qDebug()<<QString("prepare sql query [%1] error [%2]").arg (str).arg (q.lastError ().text ());
        return false;
    }
    foreach (const QString &str, keyList) {
        QString key = QString(":%1").arg (str);
        QJsonValue value = json.value (str);
        qDebug()<<"bindvalues key="<<key<<"value="<<value;
        if (value.isObject ()) {
            QJsonObject o = value.toObject ();
//            qDebug()<<"bind to json object "<<o.toVariantMap ();
            QJsonDocument doc(o);
            q.bindValue (key, QString(doc.toJson ()));
        } else {
            q.bindValue (key, value);
        }
    }
    if (q.exec()) {
        m_objMap.insert(obj.hash(), obj);
        return true;
    }
    qDebug()<<QString("run sql [%1] error [%2]").arg (str).arg (q.lastError ().text ());
    return false;
}

bool SQLite3DAO::doUpdateMetaData(const AudioMetaObject &obj, bool skipEmptyValue)
{
    if (obj.isHashEmpty()) {
        qDebug()<<"AudioMetaObject is empty";
        return false;
    }
    if (!checkDatabase ())
        return false;

    QString column;
    QJsonObject json = obj.toObject ();
    QStringList keyList;
    foreach (const QString &key, json.keys ()) {
        QString v = json.value (key).toString ();
        if (v.isEmpty () && skipEmptyValue)
            continue;
//        column += QString ("%1 = \"%2\", ").arg (key).arg (json.value (key).toString ());
        column += QString("%1 = \":%1\", ").arg (key);
        keyList.append (key);
    }

    column = column.simplified ();
    column = column.left (column.length () - 1);

    QString str = "update ";
    str += TABLE_LIBRARY_TAG;
    str += " set ";
    str += column;
    str += QString(" where %1 = \"%2\"")
            .arg (AudioMetaObject::Object_Internal_Key_Name_Hash ())
            .arg (obj.hash ());

    QSqlQuery q;
    if (!q.prepare (str)) {
        qDebug()<<QString("prepare sql query [%1] error [%2]").arg (str).arg (q.lastError ().text ());
        return false;
    }
    foreach (const QString &str, keyList) {
        QString key = QString(":%1").arg (str);
        QString value = json.value (str).toString ();
        qDebug()<<QString("bindvalues key=[%1], value=[%2]").arg (key).arg (value);
        q.bindValue (key, value);
    }

    if (q.exec()) {
        m_objMap.insert(obj.hash(), obj);
        return true;
    }
    qDebug()<<QString("run sql [%1] error [%2]").arg (str).arg (q.lastError ().text ());
    return false;
}

//bool SQLite3DAO::fillAttribute(AudioMetaObject **meta)
//{
//    if (!meta) {
//        qDebug()<<Q_FUNC_INFO<<"No SongMetaData pointer";
//        return false;
//    }
//    if (!checkDatabase ())
//        return false;
//    AudioMetaObject *d = trackFromHash ((*meta)->hash ());
//    if (!d)
//        return false;
//    (*meta)->fillAttribute (d);
//    d->deleteLater ();
//    d = nullptr;
//    return true;
//}

//bool SQLite3DAO::deleteMetaData(const AudioMetaObject &obj)
//{
//    if (obj.isHashEmpty())
//        return false;
//    return doDeleteByHash (obj.hash ());
//}

bool SQLite3DAO::doDeleteByHash(const QString &hash)
{
    if (!checkDatabase ())
        return false;

    if (hash.isEmpty ()) {
        qDebug()<<"hash is empty";
        return false;
    }

    QString str = QString("delete from %1 where %2 = \"%3\"")
            .arg (TABLE_LIBRARY_TAG)
            .arg (AudioMetaObject::Object_Internal_Key_Name_Hash ())
            .arg (hash);
    QSqlQuery q;
    if (q.exec (str)) {
//        calcExistSongs ();
        m_objMap.remove(hash);
        m_utilityMap.remove(hash);
        m_lastPlayedMap.remove(hash);
        return true;
    }
    qDebug()<<"deleteMetaData error "<<q.lastError ().text ();
    return false;
}

AudioMetaObject SQLite3DAO::trackFromHash(const QString &hash) const
{
    if (hash.isEmpty ()) {
        qDebug()<<"Invalid hash";
        return AudioMetaObject();
    }
    if (m_objMap.contains(hash)) {
        return m_objMap.value(hash);
    }

    QString str = QString("select * from %1 where %2 = \"%3\"")
            .arg (TABLE_LIBRARY_TAG)
            .arg (AudioMetaObject::Object_Internal_Key_Name_Hash ())
            .arg (hash);
    QSqlQuery q(str, m_database);
    while (q.next ()) {
        QString str = q.value (AudioMetaObject::Object_Internal_Key_Name_Hash ()).toString ();
        if (str != hash)
            continue;

        AudioMetaObject audio; //a tmp object to get the query keys
        QJsonObject json;
        foreach (const QString &key, audio.toObject ().keys ()) {
            json.insert (key, q.value (key).toString ());
        }
        QJsonDocument doc(json);
        return AudioMetaObject::fromJson(doc.toJson());
    }
    qDebug()<<"Current hash ["<<hash<<"] not found in database";
    return AudioMetaObject();
}

QStringList SQLite3DAO::trackHashList() const
{
    return m_objMap.keys();
}

bool SQLite3DAO::setLike(const QString &hash, bool like)
{
    if (m_utilityMap.contains(hash)) {
        QString str = "update ";
        str += TABLE_UTILITY_TAG;
        str += " set ";
        str += QString(" %1 = '%2' ").arg(UTILITY_KEY_LIKE).arg(like);
        str += QString(" where %1 = '%2'").arg(AudioMetaObject::Object_Internal_Key_Name_Hash()).arg(hash);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            InnerNode node = m_utilityMap.value(hash);
            node.d.data()->like = like;
            m_utilityMap.insert(hash, node);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    } else {
        QString str = "insert into ";
        str += TABLE_UTILITY_TAG;
        str += QString(" (%1, %2, %3) ")
                  .arg(AudioMetaObject::Object_Internal_Key_Name_Hash())
                  .arg(UTILITY_KEY_CNT)
                  .arg(UTILITY_KEY_LIKE);
        str += " values ";
        str += QString("('%1', '%2', '%3' ) ").arg(hash).arg(0).arg(like);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            InnerNode node;
            node.d.data()->like = like;
            m_utilityMap.insert(hash, node);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    }
    return false;
}

bool SQLite3DAO::isLike(const QString &hash) const
{
    return m_utilityMap.value(hash).d.data()->like;
}

bool SQLite3DAO::setPlayedCount(const QString &hash, int count)
{
    if (m_utilityMap.contains(hash)) {
        QString str = "update ";
        str += TABLE_UTILITY_TAG;
        str += " set ";
        str += QString(" %1 = '%2' ").arg(UTILITY_KEY_CNT).arg(count);
        str += QString(" where %1 = '%2'").arg(AudioMetaObject::Object_Internal_Key_Name_Hash()).arg(hash);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            InnerNode node = m_utilityMap.value(hash);
            node.d.data()->cnt = count;
            m_utilityMap.insert(hash, node);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    } else {
        QString str = "insert into ";
        str += TABLE_UTILITY_TAG;
        str += QString(" (%1, %2, %3) ")
                  .arg(AudioMetaObject::Object_Internal_Key_Name_Hash())
                  .arg(UTILITY_KEY_CNT)
                  .arg(UTILITY_KEY_LIKE);
        str += " values ";
        str += QString("('%1', '%2', '%3' ) ").arg(hash).arg(count).arg(false);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            InnerNode node;
            node.d.data()->cnt = count;
            m_utilityMap.insert(hash, node);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    }
    return false;
}

int SQLite3DAO::playedCount(const QString &hash) const
{
    return m_utilityMap.value(hash).d.data()->cnt;
}

bool SQLite3DAO::setLastPlayedTime(const QString &hash, qint64 secs)
{
    if (m_lastPlayedMap.keys().contains(hash)) {
        QString str = "update ";
        str += TABLE_LAST_PLAYED;
        str += " set ";
        str += QString(" %1 = '%2' ").arg(LP_KEY_TIMESTAMP).arg(secs);
        str += QString(" where %1 = '%2'").arg(LP_KEY_HASH).arg(hash);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            auto obj = m_lastPlayedMap.value(hash);
            obj.setTimestamp(secs);
            m_lastPlayedMap.insert(hash, obj);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    } else {
        AudioMetaObject obj = trackFromHash(hash);
        if (obj.isHashEmpty()) {
            return false;
        }
        QString str = "insert into ";
        str += TABLE_LAST_PLAYED;
        str += QString(" (%1, %2, %3, %4) ")
                  .arg(LP_KEY_HASH)
                  .arg(LP_KEY_ALBUM_NAME)
                  .arg(LP_KEY_ARTIST_NAME)
                  .arg(LP_KEY_GENRES)
                  .arg(LP_KEY_TIMESTAMP);
        str += " values ";
        str += QString("('%1', '%2', '%3', '%4' ) ")
                .arg(hash)
                .arg(obj.albumMeta().name())
                .arg(obj.artistMeta().name())
                .arg(obj.trackMeta().genre())
                .arg(secs);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            LastPlayedMeta meta = LastPlayedMeta::fromAudioMetaObject(obj);
            meta.setTimestamp(secs);
            m_lastPlayedMap.insert(hash, meta);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    }
    return false;
}

qint64 SQLite3DAO::getLastPlayedTime(const QString &hash) const
{
    return m_lastPlayedMap.value(hash).timestamp();
}

bool SQLite3DAO::setLastPlayedTime(const LastPlayedMeta &meta)
{
    const QString hash = meta.audioMetaObjHash();
    if (m_lastPlayedMap.keys().contains(hash)) {
        QString str = "update ";
        str += TABLE_LAST_PLAYED;
        str += " set ";
        str += QString(" %1 = '%2' ").arg(LP_KEY_TIMESTAMP).arg(meta.timestamp());
        str += QString(" where %1 = '%2'").arg(LP_KEY_HASH).arg(hash);
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            m_lastPlayedMap.insert(hash, meta);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    } else {
        QString str = "insert into ";
        str += TABLE_LAST_PLAYED;
        str += QString(" (%1, %2, %3, %4) ")
                  .arg(LP_KEY_HASH)
                  .arg(LP_KEY_ALBUM_NAME)
                  .arg(LP_KEY_ARTIST_NAME)
                  .arg(LP_KEY_GENRES)
                  .arg(LP_KEY_TIMESTAMP);
        str += " values ";
        str += QString("('%1', '%2', '%3', '%4' ) ")
                .arg(hash)
                .arg(meta.albumName())
                .arg(meta.artistName())
                .arg(meta.genres())
                .arg(meta.timestamp());
        QSqlQuery q(str, m_database);
        if (q.exec()) {
            m_lastPlayedMap.insert(hash, meta);
            return true;
        }
        qDebug()<<QString("run sql [%1] error [%2]").arg(str).arg(q.lastError().text());
    }
    return false;
}

LastPlayedMeta SQLite3DAO::getLastPlayedMeta(const QString &hash) const
{
    return m_lastPlayedMap.value(hash);
}

QList<LastPlayedMeta> SQLite3DAO::getLastPlayedMeta(int limit, bool orderByDesc) const
{
    QList<LastPlayedMeta> list = m_lastPlayedMap.values();
    if (orderByDesc) {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaGreaterThan);
    } else {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaLessThan);
    }
    if (limit <= list.size()) {
        return list.mid(0, limit);
    } else {
        return list;
    }
}

QList<LastPlayedMeta> SQLite3DAO::getLastPlayedByAlbum(int limit, bool orderByDesc) const
{
    QList<LastPlayedMeta> list = m_lastPlayedMap.values();
    if (orderByDesc) {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaGreaterThan);
    } else {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaLessThan);
    }
    QStringList albumList;
    QList<LastPlayedMeta> ll;
    foreach(const LastPlayedMeta &meta, list) {
        if (albumList.size() >= limit) {
            break;
        }
        if (!albumList.contains(meta.albumName())) {
            albumList.append(meta.albumName());
        }
        ll.append(meta);
    }
    return ll;
}

QList<LastPlayedMeta> SQLite3DAO::getLastPlayedByArtist(int limit, bool orderByDesc) const
{
    QList<LastPlayedMeta> list = m_lastPlayedMap.values();
    if (orderByDesc) {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaGreaterThan);
    } else {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaLessThan);
    }
    QStringList artistList;
    QList<LastPlayedMeta> ll;
    foreach(const LastPlayedMeta &meta, list) {
        if (artistList.size() >= limit) {
            break;
        }
        if (!artistList.contains(meta.artistName())) {
            artistList.append(meta.artistName());
        }
        ll.append(meta);
    }
    return ll;
}

QList<LastPlayedMeta> SQLite3DAO::getLastPlayedByGenres(int limit, bool orderByDesc) const
{
    QList<LastPlayedMeta> list = m_lastPlayedMap.values();
    if (orderByDesc) {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaGreaterThan);
    } else {
        std::sort(list.begin(), list.end(), sortLastPlayedMetaLessThan);
    }
    QStringList genresList;
    QList<LastPlayedMeta> ll;
    foreach(const LastPlayedMeta &meta, list) {
        if (genresList.size() >= limit) {
            break;
        }
        if (!genresList.contains(meta.genres())) {
            genresList.append(meta.genres());
        }
        ll.append(meta);
    }
    return ll;
}

QStringList SQLite3DAO::trackHashListByPlayedCount(bool orderByDesc) const
{
    QList<InnerNode> list = m_utilityMap.values();
    if (orderByDesc) {
        std::sort(list.begin(), list.end(),
                  [](const InnerNode &a, const InnerNode &b)->bool {
            return a.d.data()->cnt > b.d.data()->cnt;
        });
    } else {
        std::sort(list.begin(), list.end(),
                  [](const InnerNode &a, const InnerNode &b)->bool {
            return a.d.data()->cnt < b.d.data()->cnt;
        });
    }
    QStringList sl;
    foreach(const InnerNode &node, list) {
        sl.append(node.d.data()->hash);
    }
    return sl;
}

QStringList SQLite3DAO::trackHashListByLastPlayedTime(bool orderByDesc) const
{
    QList<LastPlayedMeta> list = m_lastPlayedMap.values();
    if (orderByDesc) {
        std::sort(list.begin(), list.end(),
                  [](const LastPlayedMeta &a, const LastPlayedMeta &b)->bool {
            return a.timestamp() > b.timestamp();
        });
    } else {
        std::sort(list.begin(), list.end(),
                  [](const LastPlayedMeta &a, const LastPlayedMeta &b)->bool {
            return a.timestamp() < b.timestamp();
        });
    }
    QStringList sl;
    foreach(const LastPlayedMeta &node, list) {
        sl.append(node.audioMetaObjHash());
    }
    return sl;
}

void SQLite3DAO::insertSpectrumData(const AudioMetaObject &obj, const QList<QList<qreal> > &list)
{
#if 0
    if (obj.isHashEmpty()) {
        qWarning()<<"Ignore insert empty hash object";
        return;
    }
    if (!checkDatabase()) {
        qWarning()<<"Database not open !!";
    }
    const QString hash = obj.hash();
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << list;

    bool isExists = false;
    {
        const QString str = QString("select * from %1 where %2 = '%3'")
                .arg(TABLE_SPECTRUM).arg(SP_KEY_HASH).arg(obj.hash());
        QSqlQuery q(str, m_database);
        while (q.next()) {
            isExists = true;
            break;
        }
    }
    if (isExists) { //update
        const QString str = QString("update %1 set %2 = ':vaData' where %3 = '%4'")
                .arg(TABLE_SPECTRUM)
                .arg(SP_KEY_DATA)
                .arg(SP_KEY_HASH)
                .arg(obj.hash());
        QSqlQuery q(m_database);
        if (!q.prepare(str)) {
            qDebug()<<"prepare sql "<<str<<" error "<<q.lastError().text();
        }
        q.bindValue(":vaData", ba);
        if (!q.exec()) {
            qDebug()<<"run sql "<<str<<" error "<<q.lastError().text();
        }
    } else {
        const QString str = QString("insert into %1 (%2, %3) values (:va1, :va2)")
                .arg(TABLE_SPECTRUM)
                .arg(SP_KEY_HASH)
                .arg(SP_KEY_DATA);

        QSqlQuery q;
        q.prepare(str);
        q.bindValue(":va1", obj.hash());
        q.bindValue(":va2", ba);
        if (!q.exec()) {
            qDebug()<<"run sql "<<str<<" error "<<q.lastError().text();
        }
    }
#endif
}

QList<QList<qreal> > SQLite3DAO::getSpectrumData(const AudioMetaObject &obj) const
{
#if 0
    if (obj.isHashEmpty()) {
        return QList<QList<qreal> >();
    }
    const QString str = QString("select %1 from %2 where %3 = '%4'")
            .arg(SP_KEY_DATA)
            .arg(TABLE_SPECTRUM)
            .arg(SP_KEY_HASH)
            .arg(obj.hash());
    QSqlQuery q(str, m_database);
    if (!q.exec()) {
        qDebug()<<"run sql "<<str<<" error "<<q.lastError().text();
    }
    while (q.next()) {
        QByteArray ba = q.value(SP_KEY_DATA).toByteArray();
        QDataStream ds(&ba, QIODevice::ReadOnly);
        QList<QList<qreal> > list;
        ds >> list;
        return list;
    }
    return QList<QList<qreal> >();
#endif
    return QList<QList<qreal> >();
}

bool SQLite3DAO::openDataBase()
{
    return initDataBase ();
}

PluginProperty SQLite3DAO::property() const
{
    return PluginProperty("SQLite3_backend",
                          "1.0",
                          "MusicLibraryDAO based on SQLite3",
                          false,
                          false);

}

//bool SQLite3DAO::deleteMetaData(PhoenixPlayer::SongMetaData *metaData)
//{
//    if (metaData == nullptr) {
//        qDebug()<<"Can't delete meta data due to metaData is empty";
//        return false;
//    }
//    return deleteMetaData (metaData->getMeta (Common::E_Hash).toString ());
//}

//bool SQLite3DAO::deleteMetaData(const QString &hash)
//{
//    if (!checkDatabase ())
//        return false;

//    if (hash.isEmpty ()) {
//        qDebug()<<"hash is empty";
//        return false;
//    }

//    QString str = QString("delete from %1 where %2 = \"%3\"")
//            .arg (LIBRARY_TABLE_TAG)
//            .arg (m_common.enumToStr ("SongMetaTags", (int)Common::E_Hash))
//            .arg (hash);
//    QSqlQuery q(str, m_database);
//    if (q.exec ()) {
//        calcExistSongs ();
//        return true;
//    } else {
//        qDebug()<<"deleteMetaData error "<<q.lastError ().text ();
//        return false;
//    }
//}

//bool SQLite3DAO::updateMetaData(PhoenixPlayer::SongMetaData *metaData, bool skipEmptyValue)
//{
//    if (!checkDatabase ())
//        return false;

//    if (metaData == nullptr
//            || metaData->getMeta (Common::E_Hash).toString ().isEmpty ()) {
//        qDebug()<<"Can't update meta data due to metaData is empty or hash is empty";
//        return false;
//    }
//    SongMetaData *oriMeta
//            = querySongMeta (metaData->getMeta (Common::E_Hash).toString (),
//                             LIBRARY_TABLE_TAG);
//    int i;
//    QString str = "update ";
//    str += LIBRARY_TABLE_TAG;
//    str += " set ";
//    for (i = (int)(Common::SongMetaTags::E_Hash) + 1;
//         i < (int)(Common::SongMetaTags::E_LastFlag) -1;
//         ++i) {
//        str += QString ("%1 = \"%2\", ")
//                .arg (m_common.enumToStr ("SongMetaTags", i))
//                .arg (fillValues (metaData->getMeta (Common::SongMetaTags(i)),
//                                  oriMeta->getMeta (Common::SongMetaTags(i)),
//                                  skipEmptyValue));
//    }
//    str += QString ("%1 = \"%2\" ")
//            .arg (m_common.enumToStr ("SongMetaTags", i))
//            .arg (fillValues (metaData->getMeta (Common::SongMetaTags(i)),
//                              oriMeta->getMeta (Common::SongMetaTags(i)),
//                              skipEmptyValue));
//    str += QString("where %1 = \"%2\"")
//            .arg (m_common.enumToStr ("SongMetaTags", Common::SongMetaTags::E_Hash))
//            .arg (metaData->getMeta (Common::E_Hash).toString ());

//    qDebug()<<"Run sql "<<str;

//    QSqlQuery q(str, m_database);
//    if (q.exec ()) {
//        return true;
//    } else {
//        qDebug()<<"try to update song meta error [ "<<q.lastError ().text ()<<" ]";
//        return false;
//    }
//}

bool SQLite3DAO::beginTransaction()
{

    qDebug()<<"===============";
    if (m_transaction) {
        qWarning()<<"Current in transaction state, will ignore this call";
        return true;
    }
    if (!checkDatabase ())
        return false;

    qDebug()<<">>>>>>>>>>>>>> SQLite3 <<<<<<<<<<<<<<<<<<<<<<";

    calcExistSongs();
    calcUtilityTable();
    calcLastPlayedTable();
    m_transaction = m_database.transaction ();
    return m_transaction;
}

bool SQLite3DAO::commitTransaction()
{
    m_transaction = false;
    if (!checkDatabase ())
        return false;

    qDebug()<<">>>>>>>>>>>>>> SQLite3  <<<<<<<<<<<<<<<<<<<<<<";

    if (m_database.commit ()) {
        calcExistSongs();
        calcUtilityTable();
        calcLastPlayedTable();
        return true;
    } else {
        return false;
    }
}

//inline QStringList SQLite3DAO::songMetaDataPropertyList()
//{
//    QStringList list(AudioMetaObject::staticPropertyList ());
//    //remove unused value
////    Q_PROPERTY(MetaData::AlbumMeta* albumMeta READ albumMeta)
////    Q_PROPERTY(MetaData::ArtistMeta* artistMeta READ artistMeta)
////    Q_PROPERTY(MetaData::CoverMeta* coverMeta READ coverMeta)
////    Q_PROPERTY(MetaData::TrackMeta* trackMeta READ trackMeta)
//    if (list.contains ("albumMeta"))
//        list.removeOne ("albumMeta");
//    if (list.contains ("artistMeta"))
//        list.removeOne ("artistMeta");
//    if (list.contains ("coverMeta"))
//        list.removeOne ("coverMeta");
//    if (list.contains ("trackMeta"))
//        list.removeOne ("trackMeta");
//    return list;
//}

//bool SQLite3DAO::insertMetaData(SongMetaData *metaData, bool skipDuplicates)
//{
//    if (!checkDatabase ())
//        return false;

//    if (skipDuplicates) {
//        if (m_existSongHashes.contains (metaData->getMeta (Common::E_Hash).toString ())) {
////            qDebug()<<"skipDuplicates "<<metaData->fileName () <<" "<<metaData->hash ();
//            return true;
//        }
//    }
//    QSqlQuery q;
//    int i;
//    QString str = "insert into ";
//    str += LIBRARY_TABLE_TAG;
//    str += "(";
//    for (i = (int)Common::SongMetaTags::E_FirstFlag + 1;
//         i < (int)Common::SongMetaTags::E_LastFlag - 1;
//         ++i) {
//        str += QString("%1, ")
//                .arg (m_common.enumToStr ("SongMetaTags", i));
//    }
//    str += QString("%1) values ( ")
//            .arg (m_common.enumToStr ("SongMetaTags", i));

//    for (i = (int)Common::SongMetaTags::E_FirstFlag + 1;
//         i < (int)Common::SongMetaTags::E_LastFlag - 1;
//         ++i) {
//        str += QString("\"%1\", ")
//                .arg (metaData->getMeta (Common::SongMetaTags(i)).toString ());
//    }
//    str += QString(" \"%1\") ")
//            .arg (metaData->getMeta (Common::SongMetaTags(i)).toString ());

//    if (q.exec (str)) {
//        m_existSongHashes.append (metaData->getMeta (Common::SongMetaTags::E_Hash)
//                                 .toString ());
//        return true;
//    } else {
//        qDebug()<<"try to insert file error [ "<<str<<" ]";
//        return false;
//    }
//}

//PhoenixPlayer::SongMetaData *SQLite3DAO::querySongMeta(const QString &hash,
//                                                       const QString &table)
//{
//    if (!checkDatabase ())
//        return 0;

//    if (hash.isEmpty () || table.isEmpty ())
//        return 0;
//    QString str = QString("select * from %1 where %2 = \"%3\"")
//            .arg (table)
//            .arg (mCommon.enumToStr ("SongMetaTags", (int)Common::E_Hash))
//            .arg (hash);
//    QSqlQuery q(str, mDatabase);
//    SongMetaData *meta = new SongMetaData(this);
//    while (q.next ()) {
////        Hash,FilePath, FileName,MediaBitrate, FileSize, ArtistName, ArtistImageUri, ArtistDescription,
////        AlbumName, AlbumDescription, AlbumYear, CoverArtSmall,CoverArtLarge, CoverArtMiddle,
////        MediaType, SongLength,SongTitle,SongDescription, Category, Year, Date,UserRating, Keywords, Language,
////         Publisher, Copyright, Lyrics, Mood
//        for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
//             i < (int)Common::SongMetaTags::E_LastFlag;
//             ++i) {
//            meta->setMeta (Common::SongMetaTags(i),
//                          q.value (mCommon.enumToStr ("SongMetaTags", i)));
//        }

////        meta.setHash                     (q.value ("Hash").toString ());
////        meta.setFilePath                 (q.value ("FilePath").toString ());
////        meta.setFileName                 (q.value ("FileName").toString ());
////        meta.setMediaBitrate             (q.value ("MediaBitrate").toInt ());
////        meta.setFileSize                 (q.value ("FileSize").toInt ());
////        meta.setArtistName               (q.value ("ArtistName").toString ());
////        meta.setArtistImageUri           (q.value ("ArtistImageUri").toString ());
////        meta.setArtistDescription        (q.value ("ArtistDescription").toString ());
////        meta.setAlbumName                (q.value ("AlbumName").toString ());
////        meta.setAlbumDescription         (q.value ("AlbumDescription").toString ());
////        meta.setAlbumYear                (q.value ("AlbumYear").toString ());
////        meta.setCoverArtSmall            (q.value ("CoverArtSmall").toString ());
////        meta.setCoverArtLarge            (q.value ("CoverArtLarge").toString ());
////        meta.setCoverArtMiddle           (q.value ("CoverArtMiddle").toString ());
////        meta.setMediaType                (Common::MediaType(q.value ("MediaType").toInt ()));
////        meta.setSongLength               (q.value ("SongLength").toInt ());
////        meta.setSongTitle                (q.value ("SongTitle").toString ());
////        meta.setSongDescription          (q.value ("SongDescription").toString ());
////        meta.setCategory                 (q.value ("Category").toString ().split ("||"));
////        meta.setYear                     (q.value ("Year").toInt ());
////        meta.setDate                     (q.value ("Date").toDate ());
////        meta.setUserRating               (q.value ("UserRating").toInt ());
////        meta.setKeywords                 (q.value ("Keywords").toString ().split ("||"));
////        meta.setLanguage                 (q.value ("Language").toString ());
////        meta.setPublisher                (q.value ("Publisher").toString ());
////        meta.setCopyright                (q.value ("Copyright").toString ());
////        meta.setLyrics                   (q.value ("Lyrics").toString ());
////        meta.setMood                     (q.value ("Mood").toString ());
//        break; //只需要第一条就够了
//    }
//    return meta;
//}

//QStringList SQLite3DAO::getSongHashList(const QString &playListHash)
//{
//    if (playListHash.isEmpty ())
//        return m_existSongHashes;

//    QStringList list = queryPlayList (Common::PlayListSongHashes,
//                                      Common::PlayListHash,
//                                      playListHash);
//    if (list.isEmpty ())
//        return m_existSongHashes;
//    return list.first ().split ("||");
//}

//QStringList SQLite3DAO::queryMusicLibrary(Common::SongMetaTags targetColumn,
//                                          Common::SongMetaTags regColumn,
//                                          const QString &regValue,
//                                          bool skipDuplicates)
//{
//    if (!checkDatabase ())
//        return QStringList();

//    QString str = "select ";
//    if (skipDuplicates)
//        str += " DISTINCT ";
//    str += mCommon.enumToStr ("SongMetaTags", (int)targetColumn);
//    str += " from ";
//    str += LIBRARY_TABLE_TAG;
//    if (!regValue.isEmpty ()
//            && !(regColumn == Common::E_FirstFlag || regColumn == Common::E_LastFlag)) {
//        str += " where ";
//        str += mCommon.enumToStr ("SongMetaTags", (int)regColumn);
//        str += QString(" = \"%1\" ").arg (regValue);

//    }
////    qDebug()<<__FUNCTION__<<"run sql "<<str;

//    QSqlQuery q(str, mDatabase);
//    QStringList list;
//    while (q.next ()) {
//        QVariant v = q.value(mCommon.enumToStr ("SongMetaTags", (int)targetColumn));
//        if (!v.isValid() || v.isNull())
//            continue;
//        list.append (v.toString());
//    }
//    return list;
//}

//QStringList SQLite3DAO::queryPlayList(Common::PlayListElement targetColumn,
//                                      Common::PlayListElement regColumn,
//                                      const QString &regValue)
//{
//    if (!checkDatabase ())
//        return QStringList();

//    QString str = "select ";
//    str += mCommon.enumToStr ("PlayListElement", (int)targetColumn);
//    str += " from ";
//    str += PLAYLIST_TABLE_TAG;
//    if (!regValue.isEmpty ()) {
//        str += " where ";
//        str += mCommon.enumToStr ("PlayListElement", (int)regColumn);
//        str += QString(" = \"%1\"").arg (regValue);
//    }

//    qDebug()<<Q_FUNC_INFO<<"try to run sql "<<str;

//    QSqlQuery q(str, mDatabase);
//    QStringList list;
//    while (q.next ()) {
//        list.append (q.value (mCommon.enumToStr ("PlayListElement", (int)targetColumn))
//                     .toString ());
//    }
//    if (targetColumn == Common::PlayListSongHashes) {
//        list = list.first ().split ("||", QString::SkipEmptyParts);
//    }
//    return list;
//}

//bool SQLite3DAO::updatePlayList(Common::PlayListElement targetColumn,
//                                const QString &hash,
//                                const QString &newValue,
//                                bool appendNewValues)
//{
//    if (!checkDatabase ())
//        return false;

//    if (targetColumn == Common::PlayListFirstFlag
//            || targetColumn == Common::PlayListHash
//            || hash.isEmpty () || newValue.isEmpty ())
//        return false;

//    /// appendNewValues 是否为添加到播放列表，否为从播放列表删除
//    QString targetValue = newValue;
//    //hash1||hash2||hash3
//    if (targetColumn == Common::PlayListSongHashes) {
//        if (appendNewValues) { //转换为list
//            //得到播放数据的list
//            QStringList list = queryPlayList (targetColumn, Common::PlayListHash, hash);
//            if (!list.isEmpty ()) {
//                targetValue = QString(); //重置空
//                if (list.size () == 1) {
//                    targetValue = QString("%1||%2").arg (list.first ()).arg (newValue);
//                } else {
//                    for (int i=0; i<list.size (); ++i) {
//                        targetValue += QString("%1||").arg (list.at (i));
//                    }
//                    targetValue += newValue;
//                }
//            }
//        } else {
//            //得到播放数据的list
//            QStringList list = queryPlayList (targetColumn, Common::PlayListHash, hash);
//            if (!list.isEmpty () && list.removeOne (targetValue)) {
//                targetValue = QString(); //重置空
//                if (!list.isEmpty ()) { //因为删除了一个数据,所以再次检测列表是否为空
//                    if (list.size () == 1) {
//                        targetValue = list.first ();
//                    } else {
//                        for (int i=0; i<list.size () -1; ++i) {
//                            targetValue += QString("%1||").arg (list.at (i));
//                        }
//                        targetValue += list.last ();
//                    }
//                }
//            }
//        }
//    }

//    //dirty hack, why a "||" at the first of new value
//    if (targetValue.startsWith ("||")) {
//        targetValue = targetValue.mid (2);
//    }
//    QString str = "update ";
//    str += PLAYLIST_TABLE_TAG;
//    str += " set ";
//    str += mCommon.enumToStr ("PlayListElement", (int)targetColumn);
//    str += QString(" = \"%1\" ").arg (targetValue);
//    str += QString("where %1 = \"%2\"")
//            .arg (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
//            .arg (hash);

//    qDebug()<<Q_FUNC_INFO<<" run sql "<<str;

//    QSqlQuery q(str, mDatabase);
//    if (q.exec ()) {
//        return true;
//    } else {
//        qDebug()<<"try to update song meta error [ "<<q.lastError ().text ()<<" ]";
//        return false;
//    }
//}

//bool SQLite3DAO::deletePlayList(const QString &playListHash)
//{
//    if (!checkDatabase ())
//        return false;

//    if (playListHash.isEmpty ()) {
//        qDebug()<<"hash is empty";
//        return false;
//    }

//    QString str = QString("delete from %1 where %2 = \"%3\"")
//            .arg (PLAYLIST_TABLE_TAG)
//            .arg (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
//            .arg (playListHash);
//    QSqlQuery q(str, mDatabase);
//    if (q.exec ()) {
//        return true;
//    } else {
//        qDebug()<<"delete play list error "<<q.lastError ().text ();
//        return false;
//    }
//}

//bool SQLite3DAO::insertPlayList(const QString &playListName)
//{
//    if (!checkDatabase ())
//        return false;

//    if (playListName.isEmpty ()) {
//        qDebug()<<"Can't make empty name play list";
//        return false;
//    }
//    QString hash = Util::calculateHash (playListName);

//    /*
//     * 获取当前已经存在的播放列表hash
//     */
//    QString str = QString("select %1 from %2")
//            .arg (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
//            .arg (PLAYLIST_TABLE_TAG);

//    QSqlQuery q(str, mDatabase);
//    while (q.next ()) {
//        if (hash == q.value (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
//                .toString ()) {
//            qDebug()<<"PlayList "<<playListName<<" seems exists";
//            return false;
//        }
//    }

//    str = "insert into ";
//    str += PLAYLIST_TABLE_TAG;
//    str += "(";
//    str += QString("%1, ").arg (mCommon.enumToStr ("PlayListElement", Common::PlayListHash));
//    str += QString("%1, ").arg (mCommon.enumToStr ("PlayListElement", Common::PlayListName));
//    str += QString("%1").arg (mCommon.enumToStr ("PlayListElement", Common::PlayListSongHashes));
//    str += ")";
//    str += " values(";
//    str += QString("\"%1\", ").arg (hash);
//    str += QString("\"%1\", ").arg (playListName);
//    str += QString("\"%1\"").arg (QString());
//    str += ")";

//    if (q.exec (str)) {
//        return true;
//    } else {
//        qDebug()<<"try to insert play list error [ "<<q.lastError ().text ()<<" ]";
//        return false;
//    }
//}

//QString SQLite3DAO::listToString(const QStringList &list)
//{
//    //a, b, c, d
//    if (list.isEmpty ())
//        return QString();
//    if (list.size () == 1)
//        return list.at (0);
//    QString str;
//    QString last = list.at (list.size ()-1);
//    for (int i=0; i<list.size ()-1; ++i) {
//        str += QString("%1||").arg (list.at (i));
//    }
//    str += last;
//    return str;
//}

//QStringList SQLite3DAO::stringToList(const QString &str)
//{
//    return str.split ("||");
//}

void SQLite3DAO::calcExistSongs()
{
    m_objMap.clear();

    QString str = QString("select * from %1").arg (TABLE_LIBRARY_TAG);

    QSqlQuery q(str, m_database);
    while (q.next ()) {
        const QString hash = q.value(AudioMetaObject::Object_Internal_Key_Name_Hash()).toString();
        AudioMetaObject audio;
        QJsonObject json;
        foreach (const QString &key, audio.toObject().keys()) {
            json.insert (key, q.value (key).toString ());
        }
        QJsonDocument doc(json);
        audio = AudioMetaObject::fromJson(doc.toJson());
        if (audio.hash() != hash) {
            qWarning()<<"bibibibibi, why hash not same!!!!!!";
        }
        m_objMap.insert(hash, audio);
    }
}

void SQLite3DAO::calcUtilityTable()
{
    m_utilityMap.clear();
    if (!checkDatabase ())
        return;
    QString str = QString("select * from %1").arg(TABLE_UTILITY_TAG);
    QSqlQuery q(str, m_database);
    while (q.next ()) {
        InnerNode node;
        node.d.data()->hash = q.value(AudioMetaObject::Object_Internal_Key_Name_Hash()).toString();;
        node.d.data()->cnt = q.value(UTILITY_KEY_CNT).toInt();
        node.d.data()->like = q.value(UTILITY_KEY_LIKE).toBool();
        m_utilityMap.insert(node.d.data()->hash, node);
    }
//    qDebug()<<Q_FUNC_INFO<<m_playedCntMap;
//    qDebug()<<Q_FUNC_INFO<<m_likeMap;
}

void SQLite3DAO::calcLastPlayedTable()
{
    m_lastPlayedMap.clear();
    QString str = QString("select * from %1").arg(TABLE_LAST_PLAYED);
    QSqlQuery q(str, m_database);
    while (q.next ()) {
        const QString hash = q.value(LP_KEY_HASH).toString();
        const QString abn = q.value(LP_KEY_ALBUM_NAME).toString();
        const QString arn = q.value(LP_KEY_ARTIST_NAME).toString();
        const QString genres = q.value(LP_KEY_GENRES).toString();
        const qint64 ts = q.value(LP_KEY_GENRES).toLongLong();
        LastPlayedMeta meta;
        meta.setAudioMetaObjHash(hash);
        meta.setGenres(genres);
        meta.setAlbumName(abn);
        meta.setArtistName(arn);
        meta.setTimestamp(ts);
        m_lastPlayedMap.insert(hash, meta);
    }
}

//QString SQLite3DAO::fillValues(const QVariant &value,
//                               const QVariant &defaultValue,
//                               bool skipEmptyValue)
//{
//    if (skipEmptyValue && value.toString ().isEmpty ()) {
//        return defaultValue.toString ();
//    } else {
//        return value.toString ();
//    }
//}

inline bool SQLite3DAO::checkDatabase()
{
    if (!m_database.isOpen ()) {
        if (!openDataBase ()) {
            qDebug()<<">>>>>>> Open Database error <<<<<<<<<<<<";
            return false;
        }
    }
    return true;
}


} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
