#ifndef PLAYLISTMETA_H
#define PLAYLISTMETA_H

#include <QSharedDataPointer>
#include <QString>

#include "libphoenixplayer_global.h"
#include "PPCommon.h"

namespace PhoenixPlayer {

class PlayListMetaPriv;
class LIBPHOENIXPLAYER_EXPORT PlayListMeta
{
public:
    PlayListMeta();
    PlayListMeta(const PlayListMeta &other);

    inline PlayListMeta &operator =(const PlayListMeta &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    bool operator == (const PlayListMeta &other);
    inline bool operator != (const PlayListMeta &other) {
        return !operator == (other);
    }

    QString getDir() const;
    void setDir(const QString &value);

    QString getFileName() const;
    void setFileName(const QString &value);

    QString getFileSuffix() const;
    void setFileSuffix(const QString &value);

    QString getTimeStamp() const;
    void setTimeStamp(const QString &value);

    QString getTag() const;
    void setTag(const QString &value);

    QString getAnnotation() const;
    void setAnnotation(const QString &value);

    QVariantMap toMap() const;

    static PlayListMeta fromMap(const QVariantMap &map);

private:
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
    QSharedDataPointer<PlayListMetaPriv> d;
};


} //namespace PhoenixPlayer

Q_DECLARE_METATYPE(PhoenixPlayer::PlayListMeta)

#endif // PLAYLISTMETA_H
