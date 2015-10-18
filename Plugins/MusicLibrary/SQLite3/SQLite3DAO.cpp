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
    :IMusicLibraryDAO(parent)
{
    m_transaction = false;

    initDataBase ();
}

SQLite3DAO::~SQLite3DAO()
{
    qDebug()<<Q_FUNC_INFO;

    if (m_database.isOpen ()) {
        m_database.close ();
    }
    if (!m_existSongHashes.isEmpty ())
        m_existSongHashes.clear ();

    qDebug()<<"after "<<Q_FUNC_INFO;
}

bool SQLite3DAO::initDataBase()
{
    qDebug()<<">>>>>>>>>>>>>> "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<<<<<<";

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
        m_database = QSqlDatabase::database ();
        qDebug()<<"Try to add database form connection  'MusicLibrary', error is "<<m_database.lastError ().text ();
    }

    if (!m_database.isValid ()) {
        qDebug()<<"Add database from connection 'MusicLibrary' failed";
        m_database = QSqlDatabase::addDatabase("QSQLITE");
        m_database.setDatabaseName(dbFile);
        qDebug()<<"Try to add a new connection MusicLibrary, error is "<<m_database.lastError ().text ();
    }

    if (!m_database.isValid ()) {
        qDebug()<<"OOPS, We can't connetc to sqlite database "<<m_database.lastError ().text ();
        m_database.removeDatabase (dbFile);
    }
    if (!m_database.open()) {
        qDebug()<<"Can't open mDatabase "<<m_database.lastError ().text ();
        m_database.removeDatabase (dbFile);
        return false;
    }

    /*
     * 检测数据表是否存在
     */
    QStringList tables = m_database.tables();
    if (tables.contains(LIBRARY_TABLE_TAG, Qt::CaseInsensitive)) {
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
//    for (int i = (int)(Common::SongMetaTags::E_FirstFlag) + 1;
//         i < (int)(Common::SongMetaTags::E_LastFlag) -1;
//         ++i) {
//        str += QString ("%1 TEXT, ").arg (m_common.enumToStr ("SongMetaTags", i));
//    }
//    str += QString("%1 TEXT )")
//            .arg (m_common.enumToStr ("SongMetaTags",
//                                     (int)Common::SongMetaTags::E_LastFlag -1));
    QStringList list = MetaData::AlbumMeta::staticPropertyList ();
    foreach (QString s, list) {
        str += QString ("%1_%2 TEXT, ").arg (classToKey<MetaData::AlbumMeta>()).arg (s);
    }
    list.clear ();
    list = MetaData::ArtistMeta::staticPropertyList ();
    foreach (QString s, list) {
        str += QString ("%1_%2 TEXT, ").arg (classToKey<MetaData::ArtistMeta>()).arg (s);
    }
    list.clear ();
    list = MetaData::CoverMeta::staticPropertyList ();
    foreach (QString s, list) {
        str += QString ("%1_%2 TEXT, ").arg (classToKey<MetaData::CoverMeta>()).arg (s);
    }
    list.clear ();
    list = MetaData::TrackMeta::staticPropertyList ();
    foreach (QString s, list) {
        str += QString ("%1_%2 TEXT, ").arg (classToKey<MetaData::TrackMeta>()).arg (s);
    }
    list.clear ();
    list = songMetaDataPropertyList ();
    foreach (QString s, list) {
        str += QString ("%1_%2 TEXT, ").arg (classToKey<SongMetaData>()).arg (s);
    }
    str = str.simplified ();
    str = str.left (str.length () - 1);
    str += ")";


    qDebug()<<Q_FUNC_INFO<<"run sql "<<str;
    /*
     * 如果数据表创建出现问题,直接删除整个数据库,防止和后面的检测冲突
     */
    if (!q.exec (str)) {
        qDebug()<<Q_FUNC_INFO<<QString("Create library tab error [ %1 ]").arg (q.lastError ().text ());
        m_database.removeDatabase (DATABASE_NAME);
        return false;
    }

//    str = "create table ";
//    str += PLAYLIST_TABLE_TAG;
//    str += "(";
//    str += "id integer primary key,";
//    for (int i = (int)(Common::PlayListElement::PlayListFirstFlag) + 1;
//         i < (int)(Common::PlayListElement::PlayListLastFlag) -1;
//         ++i) {
//        str += QString ("%1 TEXT,").arg (m_common.enumToStr ("PlayListElement", i));
//    }
//    str += QString("%1 TEXT )")
//            .arg (m_common.enumToStr ("PlayListElement",
//                                     (int)Common::PlayListElement::PlayListLastFlag -1));

//    if (!q.exec (str)) {
//        qDebug() << "Create playlist tab error "
//                 << " [ " << q.lastError ().text () << " ] ";
//        m_database.removeDatabase (DATABASE_NAME);
//        return false;
//    }
    return true;
}

bool SQLite3DAO::insertMetaData(SongMetaData **metaData, bool skipDuplicates)
{
    if (!metaData) {
        qDebug()<<Q_FUNC_INFO<<"No SongMetaData pointer";
        return false;
    }
    if (!checkDatabase ())
        return false;

    if (skipDuplicates) {
        if (m_existSongHashes.contains ((*metaData)->hash ())) {
            qDebug()<<"skipDuplicates "<<(*metaData)->uri ();
            return true;
        }
    }
    QString column, value;
    foreach (QString s, MetaData::AlbumMeta::staticPropertyList ()) {
        column += QString("%1_%2, ").arg (classToKey<MetaData::AlbumMeta>()).arg (s);
        value += QString("\"%1\", ").arg ((*metaData)->albumMeta ()->property (s).toString ());
    }
    foreach (QString s, MetaData::ArtistMeta::staticPropertyList ()) {
        column += QString("%1_%2, ").arg (classToKey<MetaData::ArtistMeta>()).arg (s);
        value += QString("\"%1\", ").arg ((*metaData)->artistMeta ()->property (s).toString ());
    }
    foreach (QString s, MetaData::CoverMeta::staticPropertyList ()) {
        column += QString("%1_%2, ").arg (classToKey<MetaData::CoverMeta>()).arg (s);
        value += QString("\"%1\", ").arg ((*metaData)->coverMeta ()->property (s).toString ());
    }
    foreach (QString s, MetaData::TrackMeta::staticPropertyList ()) {
        column += QString("%1_%2, ").arg (classToKey<MetaData::TrackMeta>()).arg (s);
        value += QString("\"%1\", ").arg ((*metaData)->trackMeta ()->property (s).toString ());
    }
    foreach (QString s, songMetaDataPropertyList ()) {
        column += QString("%1_%2, ").arg (classToKey<SongMetaData>()).arg (s);
        value += QString("\"%1\", ").arg ((*metaData)->property (s).toString ());
    }
    column = column.simplified ();
    column = column.left (column.length () - 1);
    value = value.simplified ();
    value = value.left (value.length () - 1);

    QString str = "insert into ";
    str += LIBRARY_TABLE_TAG;
    str += "(";
    str += column;
    str += ") values (";
    str += value;
    str += ") ";

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

    QSqlQuery q(str, m_database);
    if (q.exec ()) {
        m_existSongHashes.append ((*metaData)->hash ());
//        emit libraryChanged ();
        emit metaDataInserted ();
        return true;
    }
    qDebug()<<Q_FUNC_INFO<<QString("run sql [%1] error [%2]").arg (str).arg (q.lastError ().text ());
    return false;
}

bool SQLite3DAO::updateMetaData(SongMetaData **metaData, bool skipEmptyValue)
{
    if (!metaData) {
        qDebug()<<Q_FUNC_INFO<<"No SongMetaData pointer";
        return false;
    }
    if (!checkDatabase ())
        return false;

    QString column, value;
    foreach (QString s, MetaData::AlbumMeta::staticPropertyList ()) {
        value = (*metaData)->albumMeta ()->property (s).toString ();
        if (value.isEmpty () && skipEmptyValue)
            continue;
        column += QString ("%1_%2 = \"%3\", ")
                .arg (classToKey<MetaData::AlbumMeta>())
                .arg (s)
                .arg (value);

    }
    foreach (QString s, MetaData::ArtistMeta::staticPropertyList ()) {
        value = (*metaData)->artistMeta ()->property (s).toString ();
        if (value.isEmpty () && skipEmptyValue)
            continue;
        column += QString ("%1_%2 = \"%3\", ")
                .arg (classToKey<MetaData::ArtistMeta>())
                .arg (s)
                .arg (value);
    }
    foreach (QString s, MetaData::CoverMeta::staticPropertyList ()) {
        value = (*metaData)->coverMeta ()->property (s).toString ();
        if (value.isEmpty () && skipEmptyValue)
            continue;
        column += QString ("%1_%2 = \"%3\", ")
                .arg (classToKey<MetaData::CoverMeta>())
                .arg (s)
                .arg (value);
    }
    foreach (QString s, MetaData::TrackMeta::staticPropertyList ()) {
        value = (*metaData)->trackMeta ()->property (s).toString ();
        if (value.isEmpty () && skipEmptyValue)
            continue;
        column += QString ("%1_%2 = \"%3\", ")
                .arg (classToKey<MetaData::TrackMeta>())
                .arg (s)
                .arg (value);
    }
    foreach (QString s, songMetaDataPropertyList ()) {
        value = (*metaData)->property (s).toString ();
        if (value.isEmpty () && skipEmptyValue)
            continue;
        column += QString ("%1_%2 = \"%3\", ")
                .arg (classToKey<SongMetaData>())
                .arg (s)
                .arg (value);
    }
    column = column.simplified ();
    column = column.left (column.length () - 1);

    QString str = "update ";
    str += LIBRARY_TABLE_TAG;
    str += " set ";
    str += column;
    //%1_hash hash来自SongMetaData的hard code
    //TODO do not use hard code
    str += QString(" where %1_hash = \"%2\"")
            .arg (classToKey<SongMetaData>())
            .arg ((*metaData)->hash ());

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

    qDebug()<<Q_FUNC_INFO<<"Run sql "<<str;

    QSqlQuery q(str, m_database);
    if (q.exec ()) {
        return true;
    }
    qDebug()<<Q_FUNC_INFO<<QString("run sql [%1] error [%2]").arg (str).arg (q.lastError ().text ());
    return false;
}

bool SQLite3DAO::fillAttribute(SongMetaData **meta)
{
    if (!meta) {
        qDebug()<<Q_FUNC_INFO<<"No SongMetaData pointer";
        return false;
    }
    if (!checkDatabase ())
        return false;
    SongMetaData *d = trackFromHash ((*meta)->hash ());
    if (!d)
        return false;
    (*meta)->fillAttribute (d);
    d->deleteLater ();
    d = nullptr;
    return true;
}

bool SQLite3DAO::deleteMetaData(SongMetaData **metaData)
{
    if (!metaData)
        return false;
    return deleteByHash ((*metaData)->hash ());
}

bool SQLite3DAO::deleteByHash(const QString &hash)
{
    if (!checkDatabase ())
        return false;

    if (hash.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"hash is empty";
        return false;
    }

    //2_hash hash来自SongMetaData的hard code
    //TODO do not use hard code
    QString str = QString("delete from %1 where %2_hash = \"%3\"")
            .arg (LIBRARY_TABLE_TAG)
            .arg (classToKey<SongMetaData>())
            .arg (hash);
    QSqlQuery q(str, m_database);
    if (q.exec ()) {
        calcExistSongs ();
//        emit libraryChanged ();
        emit metaDataDeleted ();
        return true;
    }
    qDebug()<<Q_FUNC_INFO<<"deleteMetaData error "<<q.lastError ().text ();
    return false;
}

SongMetaData *SQLite3DAO::trackFromHash(const QString &hash)
{
    if (hash.isEmpty ()) {
        qDebug()<<Q_FUNC_INFO<<"Invalid hash";
        return nullptr;
    }
    //%2_hash hash来自SongMetaData的hard code
    //TODO do not use hard code
    QString str = QString("select * from %1 where %2_hash = \"%3\"")
            .arg (LIBRARY_TABLE_TAG)
            .arg (classToKey<SongMetaData>())
            .arg (hash);
    QSqlQuery q(str, m_database);
    while (q.next ()) {
        ///以下来自SongMetaData的hard code
        //TODO do not use hard code
        QString name = q.value (QString("%1_name").arg (classToKey<SongMetaData>())).toString ();
        QString path = q.value (QString("%1_path").arg (classToKey<SongMetaData>())).toString ();
        quint64 size = q.value (QString("%1_size").arg (classToKey<SongMetaData>())).toUInt ();
        SongMetaData d(path, name, size);
        if (d.hash () != hash)
            continue;
        d.setMediaType (q.value (QString("%1_mediaType").arg (classToKey<SongMetaData>())).toUInt ());
        d.setLyricsData (q.value (QString("%1_lyricsData").arg (classToKey<SongMetaData>())).toString ());
        d.setLyricsUri (q.value (QString("%1_lyricsUri").arg (classToKey<SongMetaData>())).toUrl ());
        ///以上来自SongMetaData的hard code

        foreach (QString s, MetaData::AlbumMeta::staticPropertyList ()) {
            d.albumMeta ()->setProperty (s,
                                         q.value (QString("%1_%2")
                                                  .arg (classToKey<MetaData::AlbumMeta>())
                                                  .arg (s)));

        }
        foreach (QString s, MetaData::ArtistMeta::staticPropertyList ()) {
            d.artistMeta ()->setProperty (s,
                                          q.value (QString("%1_%2")
                                                   .arg (classToKey<MetaData::ArtistMeta>())
                                                   .arg (s)));
        }
        foreach (QString s, MetaData::CoverMeta::staticPropertyList ()) {
            d.coverMeta ()->setProperty (s,
                                         q.value (QString("%1_%2")
                                                  .arg (classToKey<MetaData::CoverMeta>())
                                                  .arg (s)));
        }
        foreach (QString s, MetaData::TrackMeta::staticPropertyList ()) {
            d.trackMeta ()->setProperty (s,
                                         q.value (QString("%1_%2")
                                                  .arg (classToKey<MetaData::TrackMeta>())
                                                  .arg (s)));
        }
        return &d;
    }
    qDebug()<<Q_FUNC_INFO<<"Current hash not found in database";
    return nullptr;
}

QStringList SQLite3DAO::trackHashList() const
{
    return m_existSongHashes;
}

bool SQLite3DAO::openDataBase()
{
    return initDataBase ();
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

    qDebug()<<Q_FUNC_INFO<<"====";
    if (m_transaction) {
        qWarning()<<"Current in transaction state, will ignore this call";
        return true;
    }
    if (!checkDatabase ())
        return false;

    qDebug()<<">>>>>>>>>>>>>> SQLite3 "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<<<<<<";

    calcExistSongs();
    m_transaction = m_database.transaction ();
    return m_transaction;
}

bool SQLite3DAO::commitTransaction()
{
    m_transaction = false;
    if (!checkDatabase ())
        return false;

    qDebug()<<">>>>>>>>>>>>>> SQLite3 "<<Q_FUNC_INFO<<" <<<<<<<<<<<<<<<<<<<<<<";

    if (m_database.commit ()) {
        calcExistSongs ();
        return true;
    } else {
        return false;
    }
}

inline QStringList SQLite3DAO::songMetaDataPropertyList()
{
    QStringList list(SongMetaData::staticPropertyList ());
    //remove unused value
//    Q_PROPERTY(MetaData::AlbumMeta* albumMeta READ albumMeta)
//    Q_PROPERTY(MetaData::ArtistMeta* artistMeta READ artistMeta)
//    Q_PROPERTY(MetaData::CoverMeta* coverMeta READ coverMeta)
//    Q_PROPERTY(MetaData::TrackMeta* trackMeta READ trackMeta)
    if (list.contains ("albumMeta"))
        list.removeOne ("albumMeta");
    if (list.contains ("artistMeta"))
        list.removeOne ("artistMeta");
    if (list.contains ("coverMeta"))
        list.removeOne ("coverMeta");
    if (list.contains ("trackMeta"))
        list.removeOne ("trackMeta");
    return list;
}

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
    /*
     * 获取当前已经存储的歌曲hash
     */
    m_existSongHashes.clear ();
    if (!checkDatabase ())
        return;
    //%1_hash hash来自SongMetaData的hard code
    //TODO do not use hard code
    QString str = QString("select %1_hash from %2")
            .arg (classToKey<SongMetaData>())
            .arg (LIBRARY_TABLE_TAG);
    QSqlQuery q(str, m_database);
    while (q.next ()) {
        m_existSongHashes.append (q.value (QString("%1_hash").arg (classToKey<SongMetaData>()))
                                 .toString ());
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
            qDebug()<<">>> "<<Q_FUNC_INFO<<" <<< Open Database error";
            return false;
        }
    }
    return true;
}


} //SQLite3
} //MusicLibrary
} //PhoenixPlayer
