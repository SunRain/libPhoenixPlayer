#include "PlayerCore/PlayListObjectMgr.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>

#include "PPSettings.h"
#include "M3uPlayListFormat.h"
#include "SingletonPointer.h"
#include "LibPhoenixPlayerMain.h"


const static char *DB_NAME = "PhoenixPlayer_playlist.db";
const static char *KEY_DIR = "DIR";
const static char *KEY_FILE_NAME = "FILE_NAME";
const static char *KEY_FILE_SUFFIX = "FILE_SUFFIX";
const static char *KEY_TIME_STAMP = "TIME_STAMP";
const static char *KEY_TAG = "TAG";
const static char *KEY_ANNOTATION = "ANNOTATION";


PhoenixPlayer::PlayListObjectMgr::PlayListObjectMgr(QObject *parent)
    : QObject(parent)
{
    PPSettings set;
    m_dbPath = set.playListDBPath();
    readDatabase();
}

PhoenixPlayer::PlayListObjectMgr *PhoenixPlayer::PlayListObjectMgr::createInstance()
{
    return new PlayListObjectMgr;
}

PhoenixPlayer::PlayListObjectMgr *PhoenixPlayer::PlayListObjectMgr::instance()
{
    return Singleton<PlayListObjectMgr>::instance(PlayListObjectMgr::createInstance);
}

PhoenixPlayer::PlayListObjectMgr::~PlayListObjectMgr()
{
    saveToDatabase();
}

QList<PhoenixPlayer::PlayListMeta> PhoenixPlayer::PlayListObjectMgr::metaList() const
{
    return m_metaMap.values();
}

bool PhoenixPlayer::PlayListObjectMgr::addMeta(const PhoenixPlayer::PlayListMeta &meta)
{
    const QString key = QString("%1.%2").arg(meta.getFileName()).arg(meta.getFileSuffix());
    if (m_metaMap.contains(key)) {
        return false;
    }
    m_metaMap.insert(key, meta);
    return true;
}

void PhoenixPlayer::PlayListObjectMgr::tryAdd(const PhoenixPlayer::PlayListMeta &meta)
{
    if(addMeta(meta)) {
        emit addedMeta(meta);
    }
}

void PhoenixPlayer::PlayListObjectMgr::deleteMeta(const PhoenixPlayer::PlayListMeta &meta)
{
    const QString key = QString("%1.%2").arg(meta.getFileName()).arg(meta.getFileSuffix());
    if (m_metaMap.contains(key)) {
        m_metaMap.remove(key);
        emit deleteMeta(meta);
    }
}

void PhoenixPlayer::PlayListObjectMgr::updateMeta(const PlayListMeta &old, const PhoenixPlayer::PlayListMeta &newMeta)
{
    const QString oldKey = QString("%1.%2").arg(old.getFileName()).arg(old.getFileSuffix());
    const QString newKey = QString("%1.%2").arg(newMeta.getFileName()).arg(newMeta.getFileSuffix());
    if (oldKey != newKey) {
        m_metaMap.remove(oldKey);
    }
    m_metaMap.insert(newKey, newMeta);
    emit metaDataChanged(old, newMeta);
}

//void PhoenixPlayer::PlayListObjectMgr::refresh()
//{
//    readDatabase();
//}

void PhoenixPlayer::PlayListObjectMgr::readDatabase()
{
    m_metaMap.clear();
    QFile file(QString("%1/%2").arg(m_dbPath).arg(DB_NAME));
    if (!file.exists()) {
        qDebug()<<Q_FUNC_INFO<<"database file not exist, first run this program??";
        return;
    }
    if (!file.open(QFile::ReadOnly | QFile::Truncate)) {
        qDebug()<<Q_FUNC_INFO<<" open file error "<<file.errorString();
        return;
    }
    QTextStream stream(&file);
    QString str = stream.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug()<<Q_FUNC_INFO<<"parse database error "<<error.errorString();
        return;
    }
    QJsonArray array = doc.array();
    foreach(const QJsonValue &value, array) {
        QJsonObject obj = value.toObject();
        PlayListMeta meta;
        meta.setDir(obj.value(KEY_DIR).toString());
        meta.setTag(obj.value(KEY_TAG).toString());
        meta.setFileName(obj.value(KEY_FILE_NAME).toString());
        meta.setTimeStamp(obj.value(KEY_TIME_STAMP).toString());
        meta.setAnnotation(obj.value(KEY_ANNOTATION).toString());
        meta.setFileSuffix(obj.value(KEY_FILE_SUFFIX).toString());
        const QString key = QString("%1.%2").arg(meta.getFileName()).arg(meta.getFileSuffix());
        m_metaMap.insert(key, meta);
    }
    file.close();
}

void PhoenixPlayer::PlayListObjectMgr::saveToDatabase()
{
    QFile file(QString("%1/%2").arg(m_dbPath).arg(DB_NAME));
    if (file.exists()) {
        if (!file.remove()) {
            qDebug()<<Q_FUNC_INFO<<" remove exist database file error "<<file.errorString();
            return;
        }
    }
    if (!file.open(QFile::WriteOnly)) {
        qDebug()<<Q_FUNC_INFO<<" open to write file error "<<file.errorString();
        return;
    }
    QJsonArray array;
    foreach(const PlayListMeta &meta, m_metaMap.values()) {
        QJsonObject obj = QJsonObject::fromVariantMap(meta.toMap());
        array.append(obj);
    }
    QJsonDocument doc(array);
    QByteArray qba = doc.toJson();
    qDebug()<<Q_FUNC_INFO<<qba;

    file.write(qba);
    file.flush();
    file.close();
}



//void PhoenixPlayer::PlayListObjectMgr::refreshExistPlayLists()
//{
//    queryPlayLists();
//}

//QStringList PhoenixPlayer::PlayListObjectMgr::existPlayLists() const
//{
//    return m_existPlayLists;
//}

//void PhoenixPlayer::PlayListObjectMgr::setExistPlayLists(const QStringList &existPlayLists)
//{
//    if (m_existPlayLists == existPlayLists) {
//        return;
//    }
//    m_existPlayLists = existPlayLists;
//    emit existPlayListsChanged(existPlayLists);
//}

//void PhoenixPlayer::PlayListObjectMgr::queryPlayLists()
//{
//    QDir dir(m_playListDir);
//    if (!dir.exists ()) {
//        qWarning()<<Q_FUNC_INFO<<"No playList dir exist!!";
//        return;
//    }
//    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
//    QFileInfoList list = dir.entryInfoList ();
//    QStringList plst;
//    //TODO soupport more playlist format
//    M3uPlayListFormat f;
//    foreach (QFileInfo info, list) {
//        QString s = info.completeSuffix ().toLower ();
//        if (s.contains (f.extension ().toLower ())
//                || f.extension ().toLower ().contains (s)) {
//            plst.append (info.baseName ());
//        }
//    }
//    setExistPlayLists(plst);
//}
