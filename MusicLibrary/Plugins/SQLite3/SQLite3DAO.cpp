#include <QSqlDatabase>
#include <QSqlError>

#include <QLatin1String>
#include <QtAlgorithms>

#include <QDebug>

#include "SQLite3DAO.h"
#include "SongMetaData.h"
#include "Util.h"

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
    qDebug()<<__FUNCTION__;

    if (mDatabase.isOpen ()) {
        mDatabase.close ();
    }
    if (!mExistSongHashes.isEmpty ())
        mExistSongHashes.clear ();

    qDebug()<<"after "<<__FUNCTION__;
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

    if (!mDatabase.isValid ()) {
        qDebug()<<"Database is not valid";
        mDatabase = QSqlDatabase::database ();
        qDebug()<<"Try to add database form connection  'MusicLibrary', error is "<<mDatabase.lastError ().text ();
    }
    if (!mDatabase.isValid ()) {
        qDebug()<<"Add database from connection 'MusicLibrary' failed";
        mDatabase = QSqlDatabase::addDatabase("QSQLITE");
        mDatabase.setDatabaseName(DATABASE_NAME);
        qDebug()<<"Try to add a new connection MusicLibrary, error is "<<mDatabase.lastError ().text ();
    }

    if (!mDatabase.isValid ()) {
        qDebug()<<"OOPS, We can't connetc to sqlite database "<<mDatabase.lastError ().text ();
        mDatabase.removeDatabase (DATABASE_NAME);
        return false;
    }
    if (!mDatabase.open()) {
        qDebug()<<"Can't open mDatabase "<<mDatabase.lastError ().text ();
        mDatabase.removeDatabase (DATABASE_NAME);
        return false;
    }

    /*
     * 检测数据表是否存在
     */
    QStringList tables = mDatabase.tables();
    if (tables.contains(LIBRARY_TABLE_TAG, Qt::CaseInsensitive)
            && tables.contains(PLAYLIST_TABLE_TAG, Qt::CaseInsensitive)) {
        qDebug()<<"Found tables now, we will check exist Song Hashes!!!";
        calcExistSongs();
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
    str += LIBRARY_TABLE_TAG;
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
        mDatabase.removeDatabase (DATABASE_NAME);
        return false;
    }

//    E_PlayListHash,
//    E_PlayListName,
//    E_PlayListSongHashes
    str = "create table ";
    str += PLAYLIST_TABLE_TAG;
    str += "(";
    str += "id integer primary key,";
    str += "Hash TEXT,";
    str += "Name TEXT,";
    str += "SongHashes TEXT";
    str +=")";

    if (!q.exec (str)) {
        qDebug() << "Create playlist tab error "
                 << " [ " << q.lastError ().text () << " ] ";
        mDatabase.removeDatabase (DATABASE_NAME);
        return false;
    }
    return true;
}

bool SQLite3DAO::openDataBase()
{
    return initDataBase ();
}

bool SQLite3DAO::deleteMetaData(PhoenixPlayer::SongMetaData *metaData)
{
    if (metaData == 0) {
        qDebug()<<"Can't delete meta data due to metaData is empty";
        return false;
    }
    return deleteMetaData (metaData->hash());
}

bool SQLite3DAO::deleteMetaData(const QString &hash)
{
    if (!checkDatabase ())
        return false;

    if (hash.isEmpty ()) {
        qDebug()<<"hash is empty";
        return false;
    }

    QString str = QString("delete from %1 where Hash = '%2'").arg (LIBRARY_TABLE_TAG).arg (hash);
    QSqlQuery q(str, mDatabase);
    if (q.exec ()) {
        calcExistSongs ();
        return true;
    } else {
        qDebug()<<"deleteMetaData error "<<q.lastError ().text ();
        return false;
    }
}

bool SQLite3DAO::updateMetaData(PhoenixPlayer::SongMetaData *metaData, bool skipEmptyValue)
{
    if (!checkDatabase ())
        return false;

    if (metaData == 0 || metaData->hash ().isEmpty ()) {
        qDebug()<<"Can't update meta data due to metaData is empty or hash is empty";
        return false;
    }
    SongMetaData *oriMeta = querySongMeta (metaData->hash (), LIBRARY_TABLE_TAG);
    QString str = "update ";
    str += LIBRARY_TABLE_TAG;
    str += "set ";
    str += QString("FilePath = '%1', ").arg (fillValues (metaData->filePath (), oriMeta->filePath (), skipEmptyValue));
    str += QString("FileName = '%1', ").arg (fillValues (metaData->fileName (), oriMeta->fileName (), skipEmptyValue));
    str += QString("MediaBitrate = %1, ").arg (fillValues (metaData->mediaBitrate (), oriMeta->mediaBitrate (), skipEmptyValue));
    str += QString("FileSize = %1, ").arg (fillValues (metaData->fileSize (), oriMeta->fileSize (), skipEmptyValue));
    str += QString("ArtistName = '%1', ").arg (fillValues (metaData->artistName (), oriMeta->artistName (), skipEmptyValue));
    str += QString("ArtistImageUri = '%1', ").arg (fillValues (metaData->artistImageUri (), oriMeta->artistImageUri (), skipEmptyValue));
    str += QString("ArtistDescription = '%1', ").arg (fillValues (metaData->artistDescription (), oriMeta->artistDescription (), skipEmptyValue));
    str += QString("AlbumName = '%1', ").arg (fillValues (metaData->albumName (), oriMeta->albumName (), skipEmptyValue));
    str += QString("AlbumDescription = '%1', ").arg (fillValues (metaData->albumDescription (), oriMeta->albumDescription (), skipEmptyValue));
    str += QString("AlbumYear = '%1', ").arg (fillValues (metaData->albumYear (), oriMeta->albumYear (), skipEmptyValue));
    str += QString("CoverArtSmall = '%1', ").arg (fillValues (metaData->coverArtSmall (), oriMeta->coverArtSmall (), skipEmptyValue));
    str += QString("CoverArtLarge = '%1', ").arg (fillValues (metaData->coverArtLarge (), oriMeta->coverArtLarge (), skipEmptyValue));
    str += QString("CoverArtMiddle = '%1', ").arg (fillValues (metaData->coverArtMiddle (), oriMeta->coverArtMiddle (), skipEmptyValue));
    str += QString("MediaType = %1, ").arg (fillValues (metaData->mediaType (), oriMeta->mediaType (), skipEmptyValue));
    str += QString("SongLength = %1, ").arg (fillValues (metaData->songLength (), oriMeta->songLength (), skipEmptyValue));
    str += QString("SongTitle = '%1', ").arg (fillValues (metaData->songTitle (), oriMeta->songTitle (), skipEmptyValue));
    str += QString("SongDescription = '%1', ").arg (fillValues (metaData->songDescription (), oriMeta->songDescription (), skipEmptyValue));
    str += QString("Category = '%1', ").arg (fillValues (listToString (metaData->category ()), listToString (oriMeta->category ()), skipEmptyValue));
    str += QString("Year = %1, ").arg (fillValues (metaData->year (), oriMeta->year (), skipEmptyValue));
    str += QString("Date = '%1', ").arg (fillValues (metaData->date ().toString (), oriMeta->date ().toString (), skipEmptyValue));
    str += QString("UserRating = %1, ").arg (fillValues (metaData->userRating (), oriMeta->userRating (), skipEmptyValue));
    str += QString("Keywords = '%1', ").arg (fillValues (listToString (metaData->keywords ()), listToString (oriMeta->keywords ()), skipEmptyValue));
    str += QString("Language = '%1', ").arg (fillValues (metaData->language (), oriMeta->language (), skipEmptyValue));
    str += QString("Publisher = '%1', ").arg (fillValues (metaData->publisher (), oriMeta->publisher (), skipEmptyValue));
    str += QString("Copyright = '%1', ").arg (fillValues (metaData->copyright (), oriMeta->copyright (), skipEmptyValue));
    str += QString("Lyrics = '%1', ").arg (fillValues (metaData->lyrics (), oriMeta->lyrics (), skipEmptyValue));
    str += QString("Mood = '%1' ").arg (fillValues (metaData->mood (), oriMeta->mood (), skipEmptyValue));
    str += QString("where Hash = '%1'").arg (metaData->hash ());

    QSqlQuery q(str, mDatabase);
    if (q.exec ()) {
        return true;
    } else {
        qDebug()<<"try to update song meta error [ "<<q.lastError ().text ()<<" ]";
        return false;
    }
}

bool SQLite3DAO::beginTransaction()
{
    if (!checkDatabase ())
        return false;

    qDebug()<<">>>>>>>>>>>>>> SQLite3 "<<__FUNCTION__<<" <<<<<<<<<<<<<<<<<<<<<<";

    calcExistSongs();
    return mDatabase.transaction ();
}

bool SQLite3DAO::commitTransaction()
{
    if (!checkDatabase ())
        return false;

    qDebug()<<">>>>>>>>>>>>>> SQLite3 "<<__FUNCTION__<<" <<<<<<<<<<<<<<<<<<<<<<";

    if (mDatabase.commit ()) {
        calcExistSongs ();
        return true;
    } else {
        return false;
    }
}

bool SQLite3DAO::insertMetaData(SongMetaData *metaData, bool skipDuplicates)
{
    if (!checkDatabase ())
        return false;

    if (skipDuplicates) {
        if (mExistSongHashes.contains (metaData->hash ())) {
            qDebug()<<"skipDuplicates "<<metaData->fileName () <<" "<<metaData->hash ();
            return true;
        }
    }
    QSqlQuery q;
    QString str = "insert into ";
    str += LIBRARY_TABLE_TAG;
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
        mExistSongHashes.append (metaData->hash ());
        return true;
    } else {
        qDebug()<<"try to insert file error [ "<<str<<" ]";
        return false;
    }
}

PhoenixPlayer::SongMetaData *SQLite3DAO::querySongMeta(const QString &hash, const QString &table)
{
    if (!checkDatabase ())
        return 0;

    if (hash.isEmpty () || table.isEmpty ())
        return 0;
    QString str = QString("select * from %1 where Hash = '%2'").arg (table).arg (hash);
    QSqlQuery q(str, mDatabase);
    SongMetaData meta;
    while (q.next ()) {
//        Hash,FilePath, FileName,MediaBitrate, FileSize, ArtistName, ArtistImageUri, ArtistDescription,
//        AlbumName, AlbumDescription, AlbumYear, CoverArtSmall,CoverArtLarge, CoverArtMiddle,
//        MediaType, SongLength,SongTitle,SongDescription, Category, Year, Date,UserRating, Keywords, Language,
//         Publisher, Copyright, Lyrics, Mood
        meta.setHash                     (q.value ("Hash").toString ());
        meta.setFilePath                 (q.value ("FilePath").toString ());
        meta.setFileName                 (q.value ("FileName").toString ());
        meta.setMediaBitrate             (q.value ("MediaBitrate").toInt ());
        meta.setFileSize                 (q.value ("FileSize").toInt ());
        meta.setArtistName               (q.value ("ArtistName").toString ());
        meta.setArtistImageUri           (q.value ("ArtistImageUri").toString ());
        meta.setArtistDescription        (q.value ("ArtistDescription").toString ());
        meta.setAlbumName                (q.value ("AlbumName").toString ());
        meta.setAlbumDescription         (q.value ("AlbumDescription").toString ());
        meta.setAlbumYear                (q.value ("AlbumYear").toString ());
        meta.setCoverArtSmall            (q.value ("CoverArtSmall").toString ());
        meta.setCoverArtLarge            (q.value ("CoverArtLarge").toString ());
        meta.setCoverArtMiddle           (q.value ("CoverArtMiddle").toString ());
        meta.setMediaType                (Common::MediaType(q.value ("MediaType").toInt ()));
        meta.setSongLength               (q.value ("SongLength").toInt ());
        meta.setSongTitle                (q.value ("SongTitle").toString ());
        meta.setSongDescription          (q.value ("SongDescription").toString ());
        meta.setCategory                 (q.value ("Category").toString ().split ("||"));
        meta.setYear                     (q.value ("Year").toInt ());
        meta.setDate                     (q.value ("Date").toDate ());
        meta.setUserRating               (q.value ("UserRating").toInt ());
        meta.setKeywords                 (q.value ("Keywords").toString ().split ("||"));
        meta.setLanguage                 (q.value ("Language").toString ());
        meta.setPublisher                (q.value ("Publisher").toString ());
        meta.setCopyright                (q.value ("Copyright").toString ());
        meta.setLyrics                   (q.value ("Lyrics").toString ());
        meta.setMood                     (q.value ("Mood").toString ());
        break; //只需要第一条就够了
    }
    return &meta;
}

QStringList SQLite3DAO::getSongHashList(const QString &playListHash)
{
    if (playListHash.isEmpty ())
        return mExistSongHashes;

    QStringList list = queryPlayList (Common::E_PlayListSongHashes, Common::E_PlayListHash, playListHash);
    if (list.isEmpty ())
        return mExistSongHashes;
    return list.first ().split ("||");
}

QStringList SQLite3DAO::queryMusicLibrary(Common::MusicLibraryElement targetColumn, Common::MusicLibraryElement regColumn, const QString &regValue, bool skipDuplicates)
{
    if (!checkDatabase ())
        return QStringList();
    Common common;
    bool intFlag = false;
    if (regColumn == Common::E_MediaBitrate
            || regColumn == Common::E_FileSize
            || regColumn == Common::E_SongLength
            || regColumn == Common::E_Year
            || regColumn == Common::E_UserRating) {
        intFlag = true;
    }
    QString str = "select ";
    if (skipDuplicates)
        str += " DISTINCT ";
    str += common.enumToStr ("MusicLibraryElement", targetColumn).replace ("E_", "");
    str += " from ";
    str += LIBRARY_TABLE_TAG;
    if (!regValue.isEmpty ()) {
        str += " where ";
        str += common.enumToStr ("MusicLibraryElement", regColumn).replace ("E_", "");
        str += " = ";
        if (!intFlag) {
            str += "'";
            str += regValue;
            str += "'";
        } else {
            str += regValue.toInt ();
        }
    }
    qDebug()<<"try to run sql "<<str;
    QSqlQuery q(str, mDatabase);
    QStringList list;
    while (q.next ()) {
        list.append (q.value (common.enumToStr ("MusicLibraryElement", targetColumn).replace ("E_", "")).toString ());
    }

    return list;
}

QStringList SQLite3DAO::queryPlayList(Common::PlayListElement targetColumn, Common::PlayListElement regColumn, const QString &regValue)
{
    if (!checkDatabase ())
        return QStringList();
    Common common;

//    str += "Hash TEXT,";
//    str += "Name TEXT,";
//    str += "SongHashes TEXT";

    QString str = "select ";
    str += common.enumToStr ("PlayListElement", targetColumn).replace ("E_PlayList", "");
    str += " from ";
    str += PLAYLIST_TABLE_TAG;
    if (!regValue.isEmpty ()) {
        str += " where ";
        str += common.enumToStr ("PlayListElement", regColumn).replace ("E_PlayList", "");
        str += QString(" = '%1'").arg (regValue);
    }

    qDebug()<<"try to run sql "<<str;

    QSqlQuery q(str, mDatabase);
    QStringList list;
    while (q.next ()) {
        list.append (q.value (common.enumToStr ("PlayListElement", targetColumn).replace ("E_PlayList", "")).toString ());
    }
    if (targetColumn == Common::E_PlayListSongHashes) {
        list = list.first ().split ("||");
    }
    return list;
}

bool SQLite3DAO::updatePlayList(Common::PlayListElement targetColumn, const QString &hash, const QString &newValue, bool appendNewValues)
{
    if (!checkDatabase ())
        return false;

    if (targetColumn == Common::E_PlayListNullElement
            || hash.isEmpty () || newValue.isEmpty ())
        return true;

    QString targeValue = newValue;
    //hash1||hash2||hash3
    if (targetColumn == Common::E_PlayListSongHashes) {
        if (appendNewValues) {
            //转换为list
            QStringList list = queryPlayList (targetColumn, Common::E_PlayListHash, hash); //得到播放数据的list
            if (!list.isEmpty ()) {
                targeValue = QString(); //重置空
                if (list.size () == 1) {
                    targeValue = QString("%1||%2").arg (list.first ()).arg (newValue);
                } else {
                    for (int i=0; i<list.size () -1; ++i) {
                        targeValue += QString("%1||").arg (list.at (i));
                    }
                    targeValue += newValue;
                }
            }
        } else {
            QStringList list = queryPlayList (targetColumn, Common::E_PlayListHash, hash); //得到播放数据的list
            if (!list.isEmpty () && list.removeOne (targeValue)) {
                targeValue = QString(); //重置空
                if (!list.isEmpty ()) { //因为删除了一个数据,所以再次检测列表是否为空
                    if (list.size () == 1) {
                        targeValue = list.first ();
                    } else {
                        for (int i=0; i<list.size () -2; ++i) {
                            targeValue += QString("%1||").arg (list.at (i));
                        }
                        targeValue += list.last ();
                    }
                }
            }
        }
    }

    Common common;
    QString str = "update ";
    str += PLAYLIST_TABLE_TAG;
    str += "set ";
    str += common.enumToStr ("PlayListElement", targetColumn).replace ("E_PlayList", "");
    str += QString(" = '%1' ").arg (targeValue);
    str += QString("where Hash = '%1'").arg (hash);

    QSqlQuery q(str, mDatabase);
    if (q.exec ()) {
        return true;
    } else {
        qDebug()<<"try to update song meta error [ "<<q.lastError ().text ()<<" ]";
        return false;
    }
}

bool SQLite3DAO::deletePlayList(const QString &playListHash)
{
    if (!checkDatabase ())
        return false;

    if (playListHash.isEmpty ()) {
        qDebug()<<"hash is empty";
        return false;
    }

    QString str = QString("delete from %1 where Hash = '%2'").arg (PLAYLIST_TABLE_TAG).arg (playListHash);
    QSqlQuery q(str, mDatabase);
    if (q.exec ()) {
        return true;
    } else {
        qDebug()<<"delete play list error "<<q.lastError ().text ();
        return false;
    }
}

bool SQLite3DAO::insertPlayList(const QString &playListName)
{
    if (!checkDatabase ())
        return false;

    if (playListName.isEmpty ()) {
        qDebug()<<"Can't make empty name play list";
        return false;
    }
    QString hash = Util::calculateHash (playListName);

    /*
     * 获取当前已经存在的播放列表hash
     */
    QString str = QString("select Hash from %1").arg (PLAYLIST_TABLE_TAG);
    QSqlQuery q(str, mDatabase);
    while (q.next ()) {
        if (hash == q.value ("Hash").toString ()) {
            qDebug()<<"PlayList "<<playListName<<" seems exists";
            return false;
        }
    }

//    str += "Hash TEXT,";
//    str += "Name TEXT,";
//    str += "SongHashes TEXT";
    str = "insert into ";
    str += PLAYLIST_TABLE_TAG;
    str += "(";
    str += "Hash, ";
    str += "Name, ";
    str += "SongHashes";
    str += ")";
    str += " values(";
    str += QString("'%1', ").arg (hash);
    str += QString("'%1', ").arg (playListName);
    str += QString("'%1'").arg (QString());
    str += ")";

    if (q.exec (str)) {
        return true;
    } else {
        qDebug()<<"try to insert play list error [ "<<q.lastError ().text ()<<" ]";
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

void SQLite3DAO::calcExistSongs()
{
    /*
     * 获取当前已经存储的歌曲hash
     */
    mExistSongHashes.clear ();
    if (!checkDatabase ())
        return;
    QString str = QString("select Hash from %1").arg (LIBRARY_TABLE_TAG);
    QSqlQuery q(str, mDatabase);
    while (q.next ()) {
        mExistSongHashes.append (q.value ("Hash").toString ());
    }
}

QString SQLite3DAO::fillValues(const QString &value, const QString &defaultValue, bool skipEmptyValue)
{
    if (skipEmptyValue && value.isEmpty ()) {
        return defaultValue;
    } else {
        return value;
    }
}

int SQLite3DAO::fillValues(int value, int defaultVaule, bool skipEmptyValue)
{
    if (skipEmptyValue && value <= 0)
        return defaultVaule;
    else
        return value;
}

bool SQLite3DAO::checkDatabase()
{
    if (!mDatabase.isOpen ()) {
        if (!openDataBase ()) {
            qDebug()<<">>> "<<__FUNCTION__<<" <<< Open Database error";
            return false;
        }
    } else {
        return true;
    }
}


} //SQLite3
} //PlayList
} //PhoenixPlayer
