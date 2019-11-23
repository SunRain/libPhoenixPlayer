#include "PlayerCore/PlayListMeta.h"

#include "M3uPlayListFormat.h"

namespace PhoenixPlayer {

PlayListMeta::PlayListMeta()
    : d(new PlayListMetaPriv())
{
    //TODO use m3u as default suffix atm
    M3uPlayListFormat f;
    d.data()->fileSuffix = f.extension();
}

PlayListMeta::PlayListMeta(const PlayListMeta &other)
    : d(other.d)
{

}

bool PlayListMeta::operator ==(const PlayListMeta &other)
{
    qWarning()<<"Bibibibi ------- This compares all inner strings, "
             <<" be aware of the differences of timeStamp between two objects !!!!";
    return other.d.data()->dir == d.data()->dir &&
            other.d.data()->tag == d.data()->tag &&
            other.d.data()->fileName == d.data()->fileName &&
            other.d.data()->timeStamp == d.data()->timeStamp &&
            other.d.data()->annotation == d.data()->annotation &&
            other.d.data()->fileSuffix == d.data()->fileSuffix;
}

QString PlayListMeta::getDir() const
{
    return d.data()->dir;
}

void PlayListMeta::setDir(const QString &value)
{
    d.data()->dir = value;
}

QString PlayListMeta::getFileName() const
{
    return d.data()->fileName;
}

void PlayListMeta::setFileName(const QString &value)
{
    d.data()->fileName = value;
}

QString PlayListMeta::getFileSuffix() const
{
    return d.data()->fileSuffix;
}

void PlayListMeta::setFileSuffix(const QString &value)
{
    d.data()->fileSuffix = value;
}

QString PlayListMeta::getTimeStamp() const
{
    return d.data()->timeStamp;
}

void PlayListMeta::setTimeStamp(const QString &value)
{
    d.data()->timeStamp = value;
}

QString PlayListMeta::getTag() const
{
    return d.data()->tag;
}

void PlayListMeta::setTag(const QString &value)
{
    d.data()->tag = value;
}

QString PlayListMeta::getAnnotation() const
{
    return d.data()->annotation;
}

void PlayListMeta::setAnnotation(const QString &value)
{
    d.data()->annotation = value;
}

const static char *KEY_DIR = "DIR";
const static char *KEY_FILE_NAME = "FILE_NAME";
const static char *KEY_FILE_SUFFIX = "FILE_SUFFIX";
const static char *KEY_TIME_STAMP = "TIME_STAMP";
const static char *KEY_TAG = "TAG";
const static char *KEY_ANNOTATION = "ANNOTATION";

QVariantMap PlayListMeta::toMap() const
{
    QVariantMap map;
    map.insert(KEY_DIR, d.data()->dir);
    map.insert(KEY_TAG, d.data()->tag);
    map.insert(KEY_FILE_NAME, d.data()->fileName);
    map.insert(KEY_ANNOTATION, d.data()->annotation);
    map.insert(KEY_TIME_STAMP, d.data()->timeStamp);
    map.insert(KEY_FILE_SUFFIX, d.data()->fileSuffix);
    return map;
}

PlayListMeta PlayListMeta::fromMap(const QVariantMap &map)
{
    PlayListMeta meta;
    meta.setDir(map.value(KEY_DIR).toString());
    meta.setTag(map.value(KEY_TAG).toString());
    meta.setFileName(map.value(KEY_FILE_NAME).toString());
    meta.setTimeStamp(map.value(KEY_TIME_STAMP).toString());
    meta.setAnnotation(map.value(KEY_ANNOTATION).toString());
    meta.setFileSuffix(map.value(KEY_FILE_SUFFIX).toString());
    return meta;
}














} //namespace PhoenixPlayer
