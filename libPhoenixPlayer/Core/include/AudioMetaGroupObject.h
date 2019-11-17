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
        return /*d.data ()->name == other.d.data ()->name
                && d.data ()->img == other.d.data ()->img
                && d.data ()->list == other.d.data ()->list
                &&*/ d.data ()->hash == other.d.data ()->hash;
    }
    inline bool operator != (const AudioMetaGroupObject &other) {
        return !operator == (other);
    }
    inline QString name() const {
        return d.data ()->name;
    }
    inline QList<QUrl> imageUri() const {
        return d.data ()->imgs;
    }
    inline AudioMetaList list() const {
        return d.data ()->list;
    }
//    inline AudioMetaList *listPtr() {
//        return &d.data()->list;
//    }

    inline int count() const {
        return d.data ()->list.size ();
    }
    inline QString hash() const {
        return d.data ()->hash;
    }
    void setImageUri(const QList<QUrl> &uris) {
        d.data()->imgs = uris;
        calcHash();
    }
    void setName(const QString &name) {
        d.data ()->name = name;
        calcHash ();
    }
    void setList(const AudioMetaList &list) {
        d.data ()->list = list;
        calcHash ();
    }
    QJsonObject toObject() const;
    QByteArray toJson() const;
    QVariantMap toMap() const;

    static QString keyName();
    static QString keyList();
    static QString keyImgUri();
    static QString keyHash();

    static QList<QUrl> sortAndTrimImgs(const AudioMetaGroupObject &obj, bool orderByDesc = true);

private:
    void calcHash();
private:
    class Priv : public QSharedData
    {
    public:
        Priv() {
            name = QString();
            imgs = QList<QUrl>();
            hash = QString();
        }
//        QUrl img;
        QList<QUrl> imgs;
        QString name;
        QString hash;
        AudioMetaList list;
    };
    QSharedDataPointer<Priv> d;
};

} //PhoenixPlayer

Q_DECLARE_METATYPE(PhoenixPlayer::AudioMetaGroupObject)

#endif // AUDIOMETAGROUPOBJECT_H
