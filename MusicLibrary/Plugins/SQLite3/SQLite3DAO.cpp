#include <QSqlDatabase>
#include <QSqlError>

#include <QLatin1String>

#include <QDebug>

#include "SQLite3DAO.h"
#include "SongMetaData.h"

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
    if (!mExistSongHashes.isEmpty ())
        mExistSongHashes.clear ();
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
        db.removeDatabase (DATABASE_NAME);
        return false;
    }

    /*
     * 检测数据表是否存在
     */
    QStringList tables = db.tables();
    if (tables.contains(LIBRARY_TAG, Qt::CaseInsensitive)
            && tables.contains(PLAYLIST_TAG, Qt::CaseInsensitive)) {
        qDebug()<<"Found tables now, we will check exist Song Hashes!!!";

        /*
         * 获取当前已经存储的歌曲hash
         */
        QString str = QString("select Hash from %1").arg (LIBRARY_TAG);
        QSqlQuery q(str, db);
        while (q.next ()) {
            mExistSongHashes.append (q.value ("Hash").toString ());
        }
        return true;
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
//    str += "Hash integer primary key,";
    str += "FilePath TEXT,";
    str += "FileName TEXT,";
    str += "MediaBitrate integer,";
    str += "FileSize integer,";
    str += "ArtistName TEXT,";
    str += "ArtistImageUri TEXT,";
    str += "ArtistDescription TEXT,";
    str += "AlbumName TEXT,";
//    str += "AlbumImageUrl TEXT,";
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

    /*
     * 如果数据表创建出现问题,直接删除整个数据库,防止和后面的检测冲突
     */
    if (!q.exec (str)) {
        qDebug() << "Create library tab error "
                 << " [ " << q.lastError ().text () << " ] ";
        db.removeDatabase (DATABASE_NAME);
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
        db.removeDatabase (DATABASE_NAME);
        return false;
    }
    return true;
}

bool SQLite3DAO::openDataBase()
{
    return initDataBase ();
}

bool SQLite3DAO::deleteMetaData(SongMetaData *metaData)
{
    return true;
}

bool SQLite3DAO::deleteMetaData(const QString &hash)
{
    return true;
}

bool SQLite3DAO::updateMetaData(SongMetaData *metaData, bool skipEmptyValue)
{
    return true;
}

QVariant SQLite3DAO::match(Common::MusicLibraryElement e, const QString &condition)
{
    return QVariant();
}

bool SQLite3DAO::update(Common::MusicLibraryElement targetE, Common::MusicLibraryElement indexE, const QString &indexValue, const QVariant &newVaule)
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

bool SQLite3DAO::insertMetaData(SongMetaData *metaData, bool skipDuplicates)
{
    if (skipDuplicates) {
        if (mExistSongHashes.contains (metaData->hash ())) {
            qDebug()<<"skipDuplicates "<<metaData->fileName () <<" "<<metaData->hash ();
            return true;
        }
    }
    QSqlQuery q;
    QString str = "insert into ";
    str += LIBRARY_TAG;
    str += "(";
    str += "Hash, ";
    str += "FilePath, ";
    str += "FileName, ";
    str += "MediaBitrate, ";
    str += "FileSize, ";
    str += "ArtistName, ";
    str += "ArtistImageUri, ";
    str += "ArtistDescription, ";
    str += "AlbumName, ";
    str += "AlbumDescription, ";
    str += "AlbumYear, ";
    str += "CoverArtSmall, ";
    str += "CoverArtLarge, ";
    str += "CoverArtMiddle, ";
    str += "MediaType, ";
    str += "SongLength, ";
    str += "SongTitle, ";
    str += "SongDescription, ";
    str += "Category, ";
    str += "Year, ";
    str += "Date, ";
    str += "UserRating, ";
    str += "Keywords, ";
    str += "Language, ";
    str += "Publisher, ";
    str += "Copyright, ";
    str += "Lyrics, ";
    str += "Mood";
    str += ")";
    str += " values(";
    str += QString("'%1', ").arg (metaData->hash ());
    str += QString("'%1', ").arg (metaData->filePath ());
    str += QString("'%1', ").arg (metaData->fileName ());
    str += QString("%1, ").arg (metaData->mediaBitrate ());
    str += QString("%1, ").arg (metaData->fileSize ());
    str += QString("'%1', ").arg (metaData->artistName ());
    str += QString("'%1', ").arg (metaData->artistImageUri ());
    str += QString("'%1', ").arg (metaData->artistDescription ());
    str += QString("'%1', ").arg (metaData->albumName ());
    str += QString("'%1', ").arg (metaData->albumDescription ());
    str += QString("'%1', ").arg (metaData->albumYear ());
    str += QString("'%1', ").arg (metaData->coverArtSmall ());
    str += QString("'%1', ").arg (metaData->coverArtLarge ());
    str += QString("'%1', ").arg (metaData->coverArtMiddle ());
    str += QString("%1, ").arg (metaData->mediaType ());
    str += QString("%1, ").arg (metaData->songLength ());
    str += QString("'%1', ").arg (metaData->songTitle ());
    str += QString("'%1', ").arg (metaData->songDescription ());
    str += QString("'%1', ").arg (listToString (metaData->category ()));
    str += QString("'%1', ").arg (metaData->year ());
    str += QString("'%1', ").arg (metaData->date ().toString ());
    str += QString("%1, ").arg (metaData->userRating ());
    str += QString("'%1', ").arg (listToString (metaData->keywords ()));
    str += QString("'%1', ").arg (metaData->language ());
    str += QString("'%1', ").arg (metaData->publisher ());
    str += QString("'%1', ").arg (metaData->copyright ());
    str += QString("'%1', ").arg (metaData->lyrics ());
    str += QString("'%1'").arg (metaData->mood ());
    str += ")";

    if (q.exec (str)) {
        return true;
    } else {
        qDebug()<<"try to insert file error [ "<<str<<" ]";
        return false;
    }
}

QString SQLite3DAO::listToString(const QStringList &list)
{
    if (list.isEmpty ())
        return QString();
    if (list.size () == 1)
        return list.at (0);
    QString str;
    QString last = list.at (list.size ());
    for (int i=0; i<list.size ()-1; ++i) {
        str += QString("%1||").arg (list.at (i));
    }
    str += last;
    return str;
}

QStringList SQLite3DAO::stringToList(const QString &str)
{
    return str.split ("||");
}


} //SQLite3
} //PlayList
} //PhoenixPlayer
