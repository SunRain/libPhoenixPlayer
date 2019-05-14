#include "PlayerCore/PlayListMetaMgr.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

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


PhoenixPlayer::PlayListMetaMgr::PlayListMetaMgr(QObject *parent)
    : QObject(parent),
      m_nameSuffix(0)
{
    PPSettings set;
    m_dbPath = set.playListDBPath();
    readDatabase();
}

PhoenixPlayer::PlayListMetaMgr *PhoenixPlayer::PlayListMetaMgr::createInstance()
{
    return new PlayListMetaMgr;
}

PhoenixPlayer::PlayListMetaMgr *PhoenixPlayer::PlayListMetaMgr::instance()
{
    return Singleton<PlayListMetaMgr>::instance(PlayListMetaMgr::createInstance);
}

PhoenixPlayer::PlayListMetaMgr::~PlayListMetaMgr()
{
    saveToDatabase();
}

QList<PhoenixPlayer::PlayListMeta> PhoenixPlayer::PlayListMetaMgr::metaList() const
{
    return m_metaList;
}

bool PhoenixPlayer::PlayListMetaMgr::addMeta(const PhoenixPlayer::PlayListMeta &meta)
{
    foreach(const PlayListMeta &m, m_metaList) {
        if (m.getFileName() == meta.getFileName() && m.getFileSuffix() == meta.getFileSuffix()) {
            return false;
        }
    }
    m_metaList.insert(0, meta);
    return true;
}

void PhoenixPlayer::PlayListMetaMgr::tryAdd(const PhoenixPlayer::PlayListMeta &meta)
{
    if(addMeta(meta)) {
        emit addedMeta(meta);
    }
}

void PhoenixPlayer::PlayListMetaMgr::deleteMeta(const PhoenixPlayer::PlayListMeta &meta)
{
    if (m_metaList.removeOne(meta)) {
        emit deletedMeta(meta);
    }
}

void PhoenixPlayer::PlayListMetaMgr::updateMeta(const PlayListMeta &old, const PhoenixPlayer::PlayListMeta &newMeta)
{
//    const QString oldKey = QString("%1.%2").arg(old.getFileName()).arg(old.getFileSuffix());
//    const QString newKey = QString("%1.%2").arg(newMeta.getFileName()).arg(newMeta.getFileSuffix());
//    if (oldKey != newKey) {
//        m_metaMap.remove(oldKey);
//    }
//    m_metaMap.insert(newKey, newMeta);
    //    emit metaDataChanged(old, newMeta);
    const int idx = m_metaList.indexOf(old);
    if (idx < 0) {
        qDebug()<<Q_FUNC_INFO<<"can't find meta "<<old.getFileName();
        return;
    }
    if (m_metaList.contains(newMeta)) {
        emit metaDataChanged(NameConflict, old, newMeta);
        return;
    }
    m_metaList.removeAt(idx);
    m_metaList.insert(idx, newMeta);
    emit metaDataChanged(UpdateMetaRet::OK, old, newMeta);
}

PhoenixPlayer::PlayListMeta PhoenixPlayer::PlayListMetaMgr::create()
{
    PPSettings set;
    PlayListMeta meta;
    M3uPlayListFormat f;
    QString name(tr("playlist"));
    auto loopName = [&](const QString &name) -> bool {
        foreach(const PlayListMeta &m, m_metaList) {
            if (m.getFileName() == name) {
                return true;
            }
        }
        return false;
    };
    do {
        QString nn = QString("%1-%2").arg(name).arg(m_nameSuffix);
        if (loopName(nn)) {
            m_nameSuffix += 1;
        } else {
            break;
        }
    } while (true);
    meta.setDir(set.playListDir());
    meta.setFileName(QString("%1-%2").arg(name).arg(m_nameSuffix));
    meta.setTimeStamp(QString::number(QDateTime::currentSecsSinceEpoch()));
    meta.setFileSuffix(f.extension());
    return meta;
}

//void PhoenixPlayer::PlayListObjectMgr::refresh()
//{
//    readDatabase();
//}

void PhoenixPlayer::PlayListMetaMgr::readDatabase()
{
//    m_metaMap.clear();
    m_metaList.clear();
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
//        const QString key = QString("%1.%2").arg(meta.getFileName()).arg(meta.getFileSuffix());
//        m_metaMap.insert(key, meta);
        m_metaList.append(meta);
    }
    std::stable_sort(m_metaList.begin(), m_metaList.end(),
                     [](const PlayListMeta &a, const PlayListMeta &b) -> bool {
        return a.getTimeStamp().toLongLong() < b.getTimeStamp().toLongLong();

    });
    file.close();
}

void PhoenixPlayer::PlayListMetaMgr::saveToDatabase()
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
    foreach(const PlayListMeta &meta, m_metaList) {
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
