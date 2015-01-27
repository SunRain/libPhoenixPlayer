
#include "backend/BaseMediaObject.h"

namespace PhoenixPlayer {
namespace PlayBackend {
BaseMediaObject::BaseMediaObject(QObject *parent) : QObject(parent)
{
    mFilePath = QString();
    mFileName = QString();
    mMediaType = Common::TypeLocalFile;
}

BaseMediaObject::~BaseMediaObject()
{

}

BaseMediaObject *BaseMediaObject::create(const QString &filePath, const QString &fileName, Common::MediaType type, QObject *parent)
{
    BaseMediaObject *obj = new BaseMediaObject(parent);
    obj->setFileName(fileName);
    obj->setFilePath(filePath);
    obj->setMediaType(type);
    return obj;
}

QString BaseMediaObject::filePath() const
{
    return mFilePath;
}

void BaseMediaObject::setFilePath(const QString &filePath)
{
    mFilePath = filePath;
}

QString BaseMediaObject::fileName() const
{
    return mFileName;
}

void BaseMediaObject::setFileName(const QString &fileName)
{
    mFileName = fileName;
}

Common::MediaType BaseMediaObject::mediaType() const
{
    return mMediaType;
}

void BaseMediaObject::setMediaType(const Common::MediaType &mediaType)
{
    mMediaType = mediaType;
}

}//PlayBackend
} //PhoenixPlayer
