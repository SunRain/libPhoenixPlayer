#ifndef AUDIOMETAGROUPOBJECT_H
#define AUDIOMETAGROUPOBJECT_H

#include <QSharedDataPointer>
#include <QVariant>

#include "libphoenixplayer_global.h"

typedef QList<PhoenixPlayer::AudioMetaObject> AudioMetaList;

namespace PhoenixPlayer {
class AudioMetaGroupObject;
}
typedef QList<PhoenixPlayer::AudioMetaGroupObject> AudioMetaGroupList;

class QJsonObject;
namespace PhoenixPlayer {

class LIBPHOENIXPLAYER_EXPORT AudioMetaGroupObject
{
public:
    AudioMetaGroupObject();
    AudioMetaGroupObject(const AudioMetaGroupObject &other);
    virtual ~AudioMetaGroupObject();

    inline AudioMetaGroupObject &operator =(const AudioMetaGroupObject &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    inline bool operator == (const AudioMetaGroupObject &other) {
        return d.data ()->name == other.d.data ()->name
                && d.data ()->list == other.d.data ()->list;
    }
    inline bool operator != (const AudioMetaGroupObject &other) {
        return !operator == (other);
    }
    inline QString name() const {
        return d.data ()->name;
    }
    inline AudioMetaList list() {
        return d.data ()->list;
    }
    inline AudioMetaList constList() const {
        return d.data ()->list;
    }
    inline int count() const {
        return d.data ()->list.size ();
    }
    void setName(const QString &name) {
        d.data ()->name = name;
    }
    void setList(const AudioMetaList &list) {
        d.data ()->list = list;
    }
    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;

    static QString keyName();
    static QString keyList();
private:
    class Priv : public QSharedData
    {
    public:
        Priv() {
            name = QString();
        }
        QString name;
        AudioMetaList list;
    };
    QSharedDataPointer<Priv> d;
};

} //PhoenixPlayer
#endif // AUDIOMETAGROUPOBJECT_H
