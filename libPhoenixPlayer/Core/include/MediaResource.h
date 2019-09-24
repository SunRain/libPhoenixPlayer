#ifndef MEDIARESOURCE_H
#define MEDIARESOURCE_H

#include <QObject>
#include <QIODevice>

#include "PPCommon.h"
#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class LIBPHOENIXPLAYER_EXPORT MediaResource : public QObject
{
    Q_OBJECT
protected:
    explicit MediaResource(const QString &getUri, QObject *parent = Q_NULLPTR);

public:
    virtual ~MediaResource();

    virtual bool initialize();
    QString getUri() const;

    virtual QIODevice *device();

    virtual bool pending() const;
    virtual bool ready() const;
    virtual PPCommon::MediaType type() const;

    static MediaResource *create(const QString &uri);

private:
    QString m_uri;
};


} //PhoenixPlayer
#endif // MEDIARESOURCE_H
