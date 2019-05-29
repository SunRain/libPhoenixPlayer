#ifndef LOCALFILEMEDIARESOURCE_H
#define LOCALFILEMEDIARESOURCE_H

#include "MediaResource.h"

#include "libphoenixplayer_global.h"
#include "PPCommon.h"

class QFile;
namespace PhoenixPlayer {

class LocalFileMediaResource : public MediaResource
{
    Q_OBJECT
public:
    explicit LocalFileMediaResource(const QString &getUri, QObject *parent = 0);
    virtual ~LocalFileMediaResource();

    // MediaResource interface
public:
    bool initialize();
    QIODevice *device();
    bool waiting() const;
    bool ready() const;
    PPCommon::MediaType type() const;

private:
    QFile *m_file;
};



} //PhoenixPlayer
#endif // LOCALFILEMEDIARESOURCE_H
