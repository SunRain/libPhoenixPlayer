#ifndef MEDIARESOURCE_H
#define MEDIARESOURCE_H

#include <QObject>
#include <QIODevice>

#include "Common.h"
#include "libphoenixplayer_global.h"

namespace PhoenixPlayer {

class LIBPHOENIXPLAYER_EXPORT MediaResource : public QObject
{
    Q_OBJECT
public:
    explicit MediaResource(const QString &getUri, QObject *parent = 0);
    virtual ~MediaResource();

    virtual bool initialize();
    QString getUri() const;

    virtual QIODevice *device();

    virtual bool waiting() const;
    virtual bool ready() const;
    virtual Common::MediaType type() const;

    static MediaResource *create(const QString &getUri, QObject *parent = 0);

private:
    QString m_uri;
};


} //PhoenixPlayer
#endif // MEDIARESOURCE_H
