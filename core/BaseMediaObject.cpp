#include "BaseMediaObject.h"

PhoenixPlayerCore::BaseMediaObject::BaseMediaObject(QObject *parent) : QObject(parent)
{

}

PhoenixPlayerCore::BaseMediaObject::~BaseMediaObject()
{

}

PhoenixPlayerCore::BaseMediaObject *PhoenixPlayerCore::BaseMediaObject::create(const QString &filePath, const QString &fileName, PhoenixPlayerCore::Common::MediaType type, QObject *parent)
{
    BaseMediaObject *obj = new BaseMediaObject(parent);
    obj->setFileName(fileName);
    obj->setFilePath(filePath);
    obj->setMediaType(type);
    return obj;
}

QString PhoenixPlayerCore::BaseMediaObject::filePath() const
{
    return mFilePath;
}

void PhoenixPlayerCore::BaseMediaObject::setFilePath(const QString &filePath)
{
    mFilePath = filePath;
}

QString PhoenixPlayerCore::BaseMediaObject::fileName() const
{
    return mFileName;
}

void PhoenixPlayerCore::BaseMediaObject::setFileName(const QString &fileName)
{
    mFileName = fileName;
}

PhoenixPlayerCore::Common::MediaType PhoenixPlayerCore::BaseMediaObject::mediaType() const
{
    return mMediaType;
}

void PhoenixPlayerCore::BaseMediaObject::setMediaType(const PhoenixPlayerCore::Common::MediaType &mediaType)
{
    mMediaType = mediaType;
}
