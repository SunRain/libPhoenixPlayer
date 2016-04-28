#include "MediaResource.h"

#include "LocalFileMediaResource.h"

namespace PhoenixPlayer {

MediaResource::MediaResource(const QString &url, QObject *parent)
    : QObject(parent)
    , m_uri(url)
{

}

MediaResource::~MediaResource()
{

}

bool MediaResource::initialize()
{
    return true;
}

QString MediaResource::getUri() const
{
    return m_uri;
}


QIODevice *MediaResource::device() {
    return nullptr;
}

bool MediaResource::waiting() const {
    return false;
}

bool MediaResource::ready() const
{
    return true;
}

Common::MediaType MediaResource::type() const {
    return Common::MediaTypeLocalFile;
}

MediaResource *MediaResource::create(const QString &uri, QObject *parent)
{
    if (uri.contains ("://")) { //local file path doesn't contain "://"
        return new LocalFileMediaResource(uri, parent);
    }
    return new MediaResource(uri, parent);
}



} //PhoenixPlayer
