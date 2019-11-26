#include "PlayListMetaMgrInternal.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>

#include "Logger.h"

#include "PPSettings.h"
#include "LibPhoenixPlayerMain.h"

#include "../PlayerCore/M3uPlayListFormat.h"

#include "PPSettingsInternal.h"

namespace PhoenixPlayer {


const static char *DB_NAME          = "PhoenixPlayer_playlist.db";
const static char *KEY_DIR          = "DIR";
const static char *KEY_FILE_NAME    = "FILE_NAME";
const static char *KEY_FILE_SUFFIX  = "FILE_SUFFIX";
const static char *KEY_TIME_STAMP   = "TIME_STAMP";
const static char *KEY_TAG          = "TAG";
const static char *KEY_ANNOTATION   = "ANNOTATION";

PlayListMetaMgrInternal::PlayListMetaMgrInternal(QSharedPointer<PPSettingsInternal> set, QObject *parent)
    : QObject(parent),
      m_nameSuffix(0),
      m_settings(set)
{
    m_dbPath = m_settings->playListDBPath();
    readDatabase();
}

PlayListMetaMgrInternal::~PlayListMetaMgrInternal()
{
    qDebug()<<"-------------------------";

    saveToDatabase();
}

QList<PlayListMeta> PlayListMetaMgrInternal::metaList() const
{
    return m_metaList;
}

bool PlayListMetaMgrInternal::addMeta(const PlayListMeta &meta)
{
    foreach(const PlayListMeta &m, m_metaList) {
        if (m.getFileName() == meta.getFileName() && m.getFileSuffix() == meta.getFileSuffix()) {
            return false;
        }
    }
    m_metaList.insert(0, meta);
    return true;
}

void PlayListMetaMgrInternal::tryAdd(const PlayListMeta &meta)
{
    if(addMeta(meta)) {
        emit metaAdded(meta);
    }
}

void PlayListMetaMgrInternal::deleteMeta(const PlayListMeta &meta)
{
    PlayListMeta mm;
    foreach(const PlayListMeta &m, m_metaList) {
        if (meta.getFileName() == m.getFileName() && meta.getFileSuffix() == m.getFileSuffix()) {
            mm = m;
            break;
        }
    }
    if (m_metaList.removeOne(mm)) {
        LOG_DEBUG()<<" removed meta "<<mm.getFileName();
        emit metaDeleted(mm);
    }
}

void PlayListMetaMgrInternal::updateMeta(const PlayListMeta &old, const PlayListMeta &newMeta)
{
    const int idx = m_metaList.indexOf(old);
    if (idx < 0) {
        LOG_DEBUG()<<"can't find meta "<<old.getFileName();
        return;
    }
    LOG_DEBUG()<<" old "<<old.getFileName()<<" new "<<newMeta.getFileName();
    bool conflicted = false;
    foreach(const PlayListMeta &meta, m_metaList) {
        if (nameConflict(meta, newMeta)) {
            conflicted = true;
            break;
        }
    }
    if (conflicted) {
        emit metaDataChanged(PlayListMetaMgr::NameConflict, old, newMeta);
        return;
    }
    m_metaList.removeAt(idx);
    m_metaList.insert(idx, newMeta);
    emit metaDataChanged(PlayListMetaMgr::OK, old, newMeta);
}

PlayListMeta PlayListMetaMgrInternal::create()
{
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
    meta.setDir(m_settings->playListDir());
    meta.setFileName(QString("%1-%2").arg(name).arg(m_nameSuffix));
    meta.setTimeStamp(QString::number(QDateTime::currentSecsSinceEpoch()));
    meta.setFileSuffix(f.extension());
    return meta;
}

void PlayListMetaMgrInternal::saveToDatabase()
{
    QFile file(QString("%1/%2").arg(m_dbPath).arg(DB_NAME));
    if (file.exists()) {
        if (!file.remove()) {
            LOG_DEBUG()<<" remove exist database file error "<<file.errorString();
            return;
        }
    }
    if (!file.open(QFile::WriteOnly)) {
        LOG_DEBUG()<<" open to write file error "<<file.errorString();
        return;
    }
    QJsonArray array;
    foreach(const PlayListMeta &meta, m_metaList) {
        QJsonObject obj = QJsonObject::fromVariantMap(meta.toMap());
        array.append(obj);
    }
    QJsonDocument doc(array);
    QByteArray qba = doc.toJson();
    file.write(qba);
    file.flush();
    file.close();
}

void PlayListMetaMgrInternal::readDatabase()
{
    m_metaList.clear();
    QFile file(QString("%1/%2").arg(m_dbPath).arg(DB_NAME));
    if (!file.exists()) {
        LOG_DEBUG()<<"database file not exist, first run this program??";
        return;
    }
    if (!file.open(QFile::ReadOnly /*| QFile::Truncate*/)) {
        LOG_DEBUG()<<" open file error "<<file.errorString();
        return;
    }
    QTextStream stream(&file);
    QString str = stream.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        LOG_DEBUG()<<"parse database error "<<error.errorString();
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



} // namespace PhoenixPlayer
