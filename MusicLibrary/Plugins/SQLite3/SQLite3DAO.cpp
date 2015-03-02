#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>
#include <QSqlQuery>

#include <QLatin1String>
#include <QtAlgorithms>
#include <QStandardPaths>
#include <QDir>

#include <QDebug>

#include "SQLite3DAO.h"
#include "SongMetaData.h"
#include "Util.h"

namespace PhoenixPlayer {
namespace MusicLibrary {
namespace SQLite3 {

SQLite3DAO::SQLite3DAO(QObject *parent)
    :IPlayListDAO(parent)
{

}

//SQLite3DAO *SQLite3DAO::getInstance()
//{
//    static SQLite3DAO s;
//    return &s;
//}

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

QString SQLite3DAO::getDescription()
{
    return QString(DESCRIPTION);
}

bool SQLite3DAO::initDataBase()
{
    qDebug()<<">>>>>>>>>>>>>> "<<__FUNCTION__<<" <<<<<<<<<<<<<<<<<<<<<<";

    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QString dbFile = QString("%1/%2").arg(dbPath).arg(DATABASE_NAME);

    qDebug()<<"===== path is "<<dbPath;

    QDir dir(dbPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dbPath)) {
            qDebug()<<__FUNCTION__<<" Can't mkdir ";
            return false;
        }
    }

    if (!mDatabase.isValid ()) {
        qDebug()<<"Database is not valid";
        mDatabase = QSqlDatabase::database ();
        qDebug()<<"Try to add database form connection  'MusicLibrary', error is "<<mDatabase.lastError ().text ();
    }

    if (!mDatabase.isValid ()) {
        qDebug()<<"Add database from connection 'MusicLibrary' failed";
        mDatabase = QSqlDatabase::addDatabase("QSQLITE");
        mDatabase.setDatabaseName(dbFile);
        qDebug()<<"Try to add a new connection MusicLibrary, error is "<<mDatabase.lastError ().text ();
    }

    if (!mDatabase.isValid ()) {
        qDebug()<<"OOPS, We can't connetc to sqlite database "<<mDatabase.lastError ().text ();
        mDatabase.removeDatabase (dbFile);
    }
    if (!mDatabase.open()) {
        qDebug()<<"Can't open mDatabase "<<mDatabase.lastError ().text ();
        mDatabase.removeDatabase (dbFile);
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
    str += "id integer primary key, ";
    for (int i = (int)(Common::SongMetaTags::E_FirstFlag) + 1;
         i < (int)(Common::SongMetaTags::E_LastFlag) -1;
         ++i) {
        str += QString ("%1 TEXT, ").arg (mCommon.enumToStr ("SongMetaTags", i));
    }
    str += QString("%1 TEXT )")
            .arg (mCommon.enumToStr ("SongMetaTags",
                                     (int)Common::SongMetaTags::E_LastFlag -1));

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
    for (int i = (int)(Common::PlayListElement::PlayListFirstFlag) + 1;
         i < (int)(Common::PlayListElement::PlayListLastFlag) -1;
         ++i) {
        str += QString ("%1 TEXT,").arg (mCommon.enumToStr ("PlayListElement", i));
    }
    str += QString("%1 TEXT )")
            .arg (mCommon.enumToStr ("PlayListElement",
                                     (int)Common::PlayListElement::PlayListLastFlag -1));

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
    if (metaData == nullptr) {
        qDebug()<<"Can't delete meta data due to metaData is empty";
        return false;
    }
    return deleteMetaData (metaData->getMeta (Common::E_Hash).toString ());
}

bool SQLite3DAO::deleteMetaData(const QString &hash)
{
    if (!checkDatabase ())
        return false;

    if (hash.isEmpty ()) {
        qDebug()<<"hash is empty";
        return false;
    }

    QString str = QString("delete from %1 where %2 = \"%3\"")
            .arg (LIBRARY_TABLE_TAG)
            .arg (mCommon.enumToStr ("SongMetaTags", (int)Common::E_Hash))
            .arg (hash);
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

    if (metaData == nullptr
            || metaData->getMeta (Common::E_Hash).toString ().isEmpty ()) {
        qDebug()<<"Can't update meta data due to metaData is empty or hash is empty";
        return false;
    }
    SongMetaData *oriMeta
            = querySongMeta (metaData->getMeta (Common::E_Hash).toString (),
                             LIBRARY_TABLE_TAG);
    int i;
    QString str = "update ";
    str += LIBRARY_TABLE_TAG;
    str += " set ";
    for (i = (int)(Common::SongMetaTags::E_Hash) + 1;
         i < (int)(Common::SongMetaTags::E_LastFlag) -1;
         ++i) {
        str += QString ("%1 = \"%2\", ")
                .arg (mCommon.enumToStr ("SongMetaTags", i))
                .arg (fillValues (metaData->getMeta (Common::SongMetaTags(i)),
                                  oriMeta->getMeta (Common::SongMetaTags(i)),
                                  skipEmptyValue));
    }
    str += QString ("%1 = \"%2\" ")
            .arg (mCommon.enumToStr ("SongMetaTags", i))
            .arg (fillValues (metaData->getMeta (Common::SongMetaTags(i)),
                              oriMeta->getMeta (Common::SongMetaTags(i)),
                              skipEmptyValue));
    str += QString("where %1 = \"%2\"")
            .arg (mCommon.enumToStr ("SongMetaTags", Common::SongMetaTags::E_Hash))
            .arg (metaData->getMeta (Common::E_Hash).toString ());

    qDebug()<<"Run sql "<<str;

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
        if (mExistSongHashes.contains (metaData->getMeta (Common::E_Hash).toString ())) {
//            qDebug()<<"skipDuplicates "<<metaData->fileName () <<" "<<metaData->hash ();
            return true;
        }
    }
    QSqlQuery q;
    int i;
    QString str = "insert into ";
    str += LIBRARY_TABLE_TAG;
    str += "(";
    for (i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag - 1;
         ++i) {
        str += QString("%1, ")
                .arg (mCommon.enumToStr ("SongMetaTags", i));
    }
    str += QString("%1) values ( ")
            .arg (mCommon.enumToStr ("SongMetaTags", i));

    for (i = (int)Common::SongMetaTags::E_FirstFlag + 1;
         i < (int)Common::SongMetaTags::E_LastFlag - 1;
         ++i) {
        str += QString("\"%1\", ")
                .arg (metaData->getMeta (Common::SongMetaTags(i)).toString ());
    }
    str += QString(" \"%1\") ")
            .arg (metaData->getMeta (Common::SongMetaTags(i)).toString ());

    if (q.exec (str)) {
        mExistSongHashes.append (metaData->getMeta (Common::SongMetaTags::E_Hash)
                                 .toString ());
        return true;
    } else {
        qDebug()<<"try to insert file error [ "<<str<<" ]";
        return false;
    }
}

PhoenixPlayer::SongMetaData *SQLite3DAO::querySongMeta(const QString &hash,
                                                       const QString &table)
{
    if (!checkDatabase ())
        return 0;

    if (hash.isEmpty () || table.isEmpty ())
        return 0;
    QString str = QString("select * from %1 where %2 = \"%3\"")
            .arg (table)
            .arg (mCommon.enumToStr ("SongMetaTags", (int)Common::E_Hash))
            .arg (hash);
    QSqlQuery q(str, mDatabase);
    SongMetaData *meta = new SongMetaData(this);
    while (q.next ()) {
//        Hash,FilePath, FileName,MediaBitrate, FileSize, ArtistName, ArtistImageUri, ArtistDescription,
//        AlbumName, AlbumDescription, AlbumYear, CoverArtSmall,CoverArtLarge, CoverArtMiddle,
//        MediaType, SongLength,SongTitle,SongDescription, Category, Year, Date,UserRating, Keywords, Language,
//         Publisher, Copyright, Lyrics, Mood
        for (int i = (int)Common::SongMetaTags::E_FirstFlag + 1;
             i < (int)Common::SongMetaTags::E_LastFlag;
             ++i) {
            meta->setMeta (Common::SongMetaTags(i),
                          q.value (mCommon.enumToStr ("SongMetaTags", i)));
        }

//        meta.setHash                     (q.value ("Hash").toString ());
//        meta.setFilePath                 (q.value ("FilePath").toString ());
//        meta.setFileName                 (q.value ("FileName").toString ());
//        meta.setMediaBitrate             (q.value ("MediaBitrate").toInt ());
//        meta.setFileSize                 (q.value ("FileSize").toInt ());
//        meta.setArtistName               (q.value ("ArtistName").toString ());
//        meta.setArtistImageUri           (q.value ("ArtistImageUri").toString ());
//        meta.setArtistDescription        (q.value ("ArtistDescription").toString ());
//        meta.setAlbumName                (q.value ("AlbumName").toString ());
//        meta.setAlbumDescription         (q.value ("AlbumDescription").toString ());
//        meta.setAlbumYear                (q.value ("AlbumYear").toString ());
//        meta.setCoverArtSmall            (q.value ("CoverArtSmall").toString ());
//        meta.setCoverArtLarge            (q.value ("CoverArtLarge").toString ());
//        meta.setCoverArtMiddle           (q.value ("CoverArtMiddle").toString ());
//        meta.setMediaType                (Common::MediaType(q.value ("MediaType").toInt ()));
//        meta.setSongLength               (q.value ("SongLength").toInt ());
//        meta.setSongTitle                (q.value ("SongTitle").toString ());
//        meta.setSongDescription          (q.value ("SongDescription").toString ());
//        meta.setCategory                 (q.value ("Category").toString ().split ("||"));
//        meta.setYear                     (q.value ("Year").toInt ());
//        meta.setDate                     (q.value ("Date").toDate ());
//        meta.setUserRating               (q.value ("UserRating").toInt ());
//        meta.setKeywords                 (q.value ("Keywords").toString ().split ("||"));
//        meta.setLanguage                 (q.value ("Language").toString ());
//        meta.setPublisher                (q.value ("Publisher").toString ());
//        meta.setCopyright                (q.value ("Copyright").toString ());
//        meta.setLyrics                   (q.value ("Lyrics").toString ());
//        meta.setMood                     (q.value ("Mood").toString ());
        break; //只需要第一条就够了
    }
    return meta;
}

QStringList SQLite3DAO::getSongHashList(const QString &playListHash)
{
    if (playListHash.isEmpty ())
        return mExistSongHashes;

    QStringList list = queryPlayList (Common::PlayListSongHashes,
                                      Common::PlayListHash,
                                      playListHash);
    if (list.isEmpty ())
        return mExistSongHashes;
    return list.first ().split ("||");
}

QStringList SQLite3DAO::queryMusicLibrary(Common::SongMetaTags targetColumn,
                                          Common::SongMetaTags regColumn,
                                          const QString &regValue,
                                          bool skipDuplicates)
{
    if (!checkDatabase ())
        return QStringList();

    QString str = "select ";
    if (skipDuplicates)
        str += " DISTINCT ";
    str += mCommon.enumToStr ("SongMetaTags", (int)targetColumn);
    str += " from ";
    str += LIBRARY_TABLE_TAG;
    if (!regValue.isEmpty ()) {
        str += " where ";
        str += mCommon.enumToStr ("SongMetaTags", (int)regColumn);
        str += QString(" = \"%1\" ").arg (regValue);

    }
    QSqlQuery q(str, mDatabase);
    QStringList list;
    while (q.next ()) {
        QVariant v = q.value(mCommon.enumToStr ("SongMetaTags", (int)targetColumn));
        if (!v.isValid() || v.isNull())
            continue;
        list.append (v.toString());
    }
    return list;
}

QStringList SQLite3DAO::queryPlayList(Common::PlayListElement targetColumn,
                                      Common::PlayListElement regColumn,
                                      const QString &regValue)
{
    if (!checkDatabase ())
        return QStringList();

    QString str = "select ";
    str += mCommon.enumToStr ("PlayListElement", (int)targetColumn);
    str += " from ";
    str += PLAYLIST_TABLE_TAG;
    if (!regValue.isEmpty ()) {
        str += " where ";
        str += mCommon.enumToStr ("PlayListElement", (int)regColumn);
        str += QString(" = \"%1\"").arg (regValue);
    }

    qDebug()<<"try to run sql "<<str;

    QSqlQuery q(str, mDatabase);
    QStringList list;
    while (q.next ()) {
        list.append (q.value (mCommon.enumToStr ("PlayListElement", (int)targetColumn))
                     .toString ());
    }
    if (targetColumn == Common::PlayListSongHashes) {
        list = list.first ().split ("||");
    }
    return list;
}

bool SQLite3DAO::updatePlayList(Common::PlayListElement targetColumn,
                                const QString &hash,
                                const QString &newValue,
                                bool appendNewValues)
{
    if (!checkDatabase ())
        return false;

    if (targetColumn == Common::PlayListFirstFlag
            || targetColumn == Common::PlayListHash
            || hash.isEmpty () || newValue.isEmpty ())
        return false;

    QString targeValue = newValue;
    //hash1||hash2||hash3
    if (targetColumn == Common::PlayListSongHashes) {
        if (appendNewValues) {
            //转换为list
            QStringList list //得到播放数据的list
                    = queryPlayList (targetColumn, Common::PlayListHash, hash);

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
            QStringList list //得到播放数据的list
                    = queryPlayList (targetColumn, Common::PlayListHash, hash);

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

    QString str = "update ";
    str += PLAYLIST_TABLE_TAG;
    str += "set ";
    str += mCommon.enumToStr ("PlayListElement", (int)targetColumn);
    str += QString(" = \"%1\" ").arg (targeValue);
    str += QString("where %1 = \"%2\"")
            .arg (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
            .arg (hash);

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

    QString str = QString("delete from %1 where %2 = \"%3\"")
            .arg (PLAYLIST_TABLE_TAG)
            .arg (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
            .arg (playListHash);
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
    QString str = QString("select %1 from %2")
            .arg (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
            .arg (PLAYLIST_TABLE_TAG);

    QSqlQuery q(str, mDatabase);
    while (q.next ()) {
        if (hash == q.value (mCommon.enumToStr ("PlayListElement", (int)Common::PlayListHash))
                .toString ()) {
            qDebug()<<"PlayList "<<playListName<<" seems exists";
            return false;
        }
    }

    str = "insert into ";
    str += PLAYLIST_TABLE_TAG;
    str += "(";
    str += QString("%1, ").arg (mCommon.enumToStr ("PlayListElement", Common::PlayListHash));
    str += QString("%1, ").arg (mCommon.enumToStr ("PlayListElement", Common::PlayListName));
    str += QString("%1, ").arg (mCommon.enumToStr ("PlayListElement", Common::PlayListSongHashes));
    str += ")";
    str += " values(";
    str += QString("\"%1\", ").arg (hash);
    str += QString("\"%1\", ").arg (playListName);
    str += QString("\"%1\"").arg (QString());
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
    //a, b, c, d
    if (list.isEmpty ())
        return QString();
    if (list.size () == 1)
        return list.at (0);
    QString str;
    QString last = list.at (list.size ()-1);
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
    QString str = QString("select %1 from %2")
            .arg (mCommon.enumToStr ("SongMetaTags", Common::E_Hash))
            .arg (LIBRARY_TABLE_TAG);
    QSqlQuery q(str, mDatabase);
    while (q.next ()) {
        mExistSongHashes.append (q.value (mCommon.enumToStr ("SongMetaTags", Common::E_Hash))
                                 .toString ());
    }
}

QString SQLite3DAO::fillValues(const QVariant &value,
                               const QVariant &defaultValue,
                               bool skipEmptyValue)
{
    if (skipEmptyValue && value.toString ().isEmpty ()) {
        return defaultValue.toString ();
    } else {
        return value.toString ();
    }
}

bool SQLite3DAO::checkDatabase()
{
    if (!mDatabase.isOpen ()) {
        if (!openDataBase ()) {
            qDebug()<<">>> "<<__FUNCTION__<<" <<< Open Database error";
            return false;
        }
    }
    return true;
}


} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
