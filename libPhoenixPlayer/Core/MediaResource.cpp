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

bool MediaResource::pending() const {
    return false;
}

bool MediaResource::ready() const
{
    return true;
}

PPCommon::MediaType MediaResource::type() const {
    return PPCommon::MediaTypeLocalFile;
}

MediaResource *MediaResource::create(const QString &uri, QObject *parent)
{
    QString str = uri;
    if (str.startsWith ("file://"))
        str = str.mid (7); //remove file://
    if (str.contains ("://")) { //local file path doesn't contain "://"
        return new LocalFileMediaResource(str, parent);
    }
    return new MediaResource(str, parent);
}



} //PhoenixPlayer
