#include <QSqlDatabase>
#include <QSqlError>

#include <QLatin1String>

#include <QDebug>

#include "SQLite3DAO.h"

namespace PhoenixPlayer {
namespace PlayList {
namespace SQLite3 {


SQLite3DAO::SQLite3DAO(QObject *parent)
    :IPlayListDAO(parent)
{

}

SQLite3DAO *SQLite3DAO::getInstance()
{
    static SQLite3DAO s;
    return &s;
}

SQLite3DAO::~SQLite3DAO()
{
    if (!mHashList.isEmpty ())
        mHashList.clear ();
    if (mDatabase.isOpen ())
        mDatabase.close ();
}

QString SQLite3DAO::getPluginName()
{
    return DAO_NAME;
}

QString SQLite3DAO::getPluginVersion()
{
    return VERSION;
}

bool SQLite3DAO::initDataBase()
{
    qDebug()<<">>>>>>>>>>>>>> "<<__FUNCTION__<<" <<<<<<<<<<<<<<<<<<<<<<";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DATABASE_NAME);

    if (!db.open()) {
        qDebug()<<"Can't open db "<<db.lastError ().text ();
        return false;
    }
    QStringList tables = db.tables();
    if (tables.contains(LIBRARY_TAG, Qt::CaseInsensitive)
            && tables.contains(PLAYLIST_TAG, Qt::CaseInsensitive)) {
        qDebug()<< QSqlError().text ();
    }

//    NULL	NULL value.	NULL
//    INTEGER	Signed integer, stored in 8, 16, 24, 32, 48, or 64-bits depending on the magnitude of the value.	typedef qint8/16/32/64
//    REAL	64-bit floating point value.	By default mapping to QString
//    TEXT	Character string (UTF-8, UTF-16BE or UTF-16-LE).	Mapped to QString
//    CLOB	Character large string object	Mapped to QString
//    BLOB	The value is a BLOB of data, stored exactly as it was input.	Mapped to QByteArray

    QSqlQuery q;
    QString str;
    str = "create table ";
    str += LIBRARY_TAG;
    str += "(";
    str += "id integer primary key,";
    str += "Hash TEXT,";
    str += "FilePath TEXT,";
    str += "FileName TEXT,";
    str += "MediaBitrate integer,";
    str += "FileSize integer,";
    str += "ArtistName TEXT,";
    str += "ArtistImageUri TEXT,";
    str += "ArtistDescription TEXT,";
    str += "AlbumName TEXT,";
    str += "AlbumImageUrl TEXT,";
    str += "AlbumDescription TEXT,";
    str += "AlbumYear TEXT,";
    str += "CoverArtSmall TEXT,";
    str += "CoverArtLarge TEXT,";
    str += "CoverArtMiddle TEXT,";
    str += "MediaType integer,";
    str += "SongLength integer,";
    str += "SongTitle TEXT,";
    str += "SongDescription TEXT,";
    str += "Category TEXT,";
    str += "Year TEXT,";
    str += "Date TEXT,";
    str += "UserRating integer,";
    str += "Keywords TEXT,";
    str += "Language TEXT,";
    str += "Publisher TEXT,";
    str += "Copyright TEXT,";
    str += "Lyrics TEXT,";
    str += "Mood TEXT";
    str +=")";

    if (!q.exec (str)) {
        qDebug() << "Create library tab error "
                 << " [ " << q.lastError ().text () << " ] ";
        return false;
    }

    str = "create table ";
    str += PLAYLIST_TAG;
    str += "(";
    str += "id integer primary key,";
    str += "Name TEXT,";
    str += "SongHashes TEXT";
    str +=")";

    if (!q.exec (str)) {
        qDebug() << "Create playlist tab error "
                 << " [ " << q.lastError ().text () << " ] ";
        return false;
    }
}

bool SQLite3DAO::openDataBase()
{
    return true;
}

bool SQLite3DAO::beginTransaction()
{
    return true;
}

bool SQLite3DAO::commitTransaction()
{
    return true;
}

bool SQLite3DAO::insertMetaData(SongMetaData *metaData)
{
    return true;
}

bool SQLite3DAO::updateMetaData(SongMetaData *metaData)
{
    return true;
}

bool SQLite3DAO::updateMetaData(const QString &hash, const QString &columnName, const QVariant &newValue)
{
    return true;
}

bool SQLite3DAO::deleteMetaData(SongMetaData *metaData)
{
    return true;
}

bool SQLite3DAO::deleteMetaData(const QString &hash)
{
    return true;
}

bool SQLite3DAO::execSqlQuery(const QSqlQuery &query)
{
    return true;
}

QVariant SQLite3DAO::query(const QString &sql)
{
    return true;
}

bool SQLite3DAO::createPlayList(const QString &playListName)
{
    return true;
}

bool SQLite3DAO::deletePlayList(const QString &playListName)
{
    return false;
}

} //SQLite3
} //PlayList
} //PhoenixPlayer
