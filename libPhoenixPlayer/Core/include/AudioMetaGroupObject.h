#ifndef AUDIOMETAGROUPOBJECT_H
#define AUDIOMETAGROUPOBJECT_H

#include <QSharedDataPointer>
#include <QVariant>

#include "libphoenixplayer_global.h"
#include "AudioMetaObject.h"

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
                && d.data ()->img == other.d.data ()->img
                && d.data ()->list == other.d.data ()->list;
    }
    inline bool operator != (const AudioMetaGroupObject &other) {
        return !operator == (other);
    }
    inline QString name() const {
        return d.data ()->name;
    }
    inline QUrl imageUri() const {
        return d.data ()->img;
    }
    inline AudioMetaList list() const {
        return d.data ()->list;
    }
    inline int count() const {
        return d.data ()->list.size ();
    }
    void setImageUri(const QUrl &uri) {
        d.data ()->img = uri;
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
    static QString keyImgUri();
private:
    class Priv : public QSharedData
    {
    public:
        Priv() {
            name = QString();
            img = QUrl();
        }
        QUrl img;
        QString name;
        AudioMetaList list;
    };
    QSharedDataPointer<Priv> d;
};

} //PhoenixPlayer
#endif // AUDIOMETAGROUPOBJECT_H
