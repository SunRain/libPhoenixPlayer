#include "PlayerCore/PlayListMeta.h"

namespace PhoenixPlayer {


class PlayListMetaPriv : public QSharedData
{
public:
    PlayListMetaPriv()
    {
        dir = QString();
        fileName = QString();
        fileSuffix = QString();
        timeStamp = QString();
        tag = QString();
        annotation = QString();
    }
    QString dir;
    QString fileName;
    QString fileSuffix;
    QString timeStamp;
    QString tag;
    QString annotation;
};

PlayListMeta::PlayListMeta()
    : d(new PlayListMetaPriv())
{

}

PlayListMeta::PlayListMeta(const PlayListMeta &other)
    : d(other.d)
{

}

bool PlayListMeta::operator ==(const PlayListMeta &other)
{
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














} //namespace PhoenixPlayer
