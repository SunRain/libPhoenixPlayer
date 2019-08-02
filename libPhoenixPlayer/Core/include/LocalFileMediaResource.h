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
    explicit LocalFileMediaResource(const QString &getUri, QObject *parent = Q_NULLPTR);
    virtual ~LocalFileMediaResource() override;

    // MediaResource interface
public:
    bool initialize() Q_DECL_OVERRIDE;
    QIODevice *device() Q_DECL_OVERRIDE;
    bool pending() const Q_DECL_OVERRIDE;
    bool ready() const Q_DECL_OVERRIDE;
    PPCommon::MediaType type() const Q_DECL_OVERRIDE;

private:
    QFile *m_file;
};



} //PhoenixPlayer
#endif // LOCALFILEMEDIARESOURCE_H
