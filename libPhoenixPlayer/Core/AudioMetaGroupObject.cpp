#include "AudioMetaGroupObject.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>

#include "AudioMetaObject.h"

const static char *KEY_NAME         = "KEY_NAME";
const static char *KEY_LIST         = "KEY_LIST";
const static char *KEY_IMG_URI      = "KEY_IMG_URI";
const static char *KEY_HASH         = "KEY_HASH";

namespace PhoenixPlayer {


class SortNode
{
public:
    SortNode() : d(new Priv()){}
    SortNode(const SortNode &other) : d(other.d) {}

    class Priv : public QSharedData
    {
    public:
        Priv() {}
        int cnt = 0;
        QUrl uri = QUrl();
     };
    QSharedDataPointer<Priv> d;

    inline SortNode &operator =(const SortNode &other) {
        if (this != &other)
            d.operator = (other.d);
        return *this;
    }
    inline bool operator == (const SortNode &other) {
        return other.d.data()->uri == d.data()->uri &&
                other.d.data()->cnt == d.data()->cnt;
    }
    inline bool operator != (const SortNode &other) {
        return !operator == (other);
    }
};

AudioMetaGroupObject::AudioMetaGroupObject()
    :d (new Priv())
{
}

AudioMetaGroupObject::AudioMetaGroupObject(const AudioMetaGroupObject &other)
    :d (other.d)
{
}

AudioMetaGroupObject::~AudioMetaGroupObject()
{
}

QJsonObject AudioMetaGroupObject::toObject() const
{
    QJsonObject o;
    o.insert (KEY_NAME, d.data ()->name);
    QJsonArray list;
    QString ud;
    foreach (const AudioMetaObject &obj, d.data ()->list) {
        ud.append (obj.hash ());
        list.append (obj.toObject ());
    }
    o.insert (KEY_HASH, d.data ()->hash);
    o.insert (KEY_LIST, list);
    QJsonArray array;
    foreach(const QUrl &u, d.data()->imgs) {
        array.append(u.toString());
    }
    o.insert(KEY_IMG_URI, array);
    return o;
}

QByteArray AudioMetaGroupObject::toJson() const
{
    QJsonDocument doc(toObject ());
    return doc.toJson ();
}

QVariantMap AudioMetaGroupObject::toMap() const
{
    QVariantMap o;
    o.insert (KEY_NAME, d.data ()->name);
    QVariantList list;
    QString ud;
    foreach (AudioMetaObject obj, d.data ()->list) {
        list.append (obj.toMap ());
        ud.append (obj.hash ());
    }
    o.insert (KEY_HASH, d.data ()->hash);
    o.insert (KEY_LIST, list);
    QVariantList array;
    foreach(const QUrl &u, d.data()->imgs) {
        array.append(u.toString());
    }
    o.insert(KEY_IMG_URI, array);
    return o;
}

QString AudioMetaGroupObject::keyName()
{
    return KEY_NAME;
}

QString AudioMetaGroupObject::keyList()
{
    return KEY_LIST;
}

QString AudioMetaGroupObject::keyImgUri()
{
    return KEY_IMG_URI;
}

QString AudioMetaGroupObject::keyHash()
{
    return KEY_HASH;
}

QList<QUrl> AudioMetaGroupObject::sortAndTrimImgs(const AudioMetaGroupObject &obj, bool orderByDesc)
{
    QMap<QUrl, int> map;
    foreach(const QUrl &uri, obj.imageUri()) {
        if (map.contains(uri)) {
            int i = map.value(uri);
            map.insert(uri, ++i);
        } else {
            map.insert(uri, 1);
        }
    }
    QList<SortNode> list;
    auto it = map.constBegin();
    while (it != map.constEnd()) {
        SortNode node;
        node.d->uri = it.key();
        node.d->cnt = it.value();
        list.append(node);
        ++it;
    }
    if (orderByDesc) {
        std::sort(list.begin(), list.end(),
                  [](const SortNode &a, const SortNode &b) -> bool{
            return a.d->cnt > b.d->cnt;
        });
    } else {
        std::sort(list.begin(), list.end(),
                  [](const SortNode &a, const SortNode &b) -> bool{
            return a.d->cnt < b.d->cnt;
        });
    }
    QList<QUrl> ll;
    foreach(const SortNode &node, list) {
        ll.append(node.d->uri);
    }
    return ll;
}

void AudioMetaGroupObject::calcHash()
{
    QString str;
//    str.append (d.data ()->img.toString ());
    foreach(const QUrl &s, d.data()->imgs) {
        str.append(s.toString());
    }
    str.append (d.data ()->name);
    foreach (const AudioMetaObject &obj, d.data ()->list) {
        str.append (obj.hash ());
    }
    d.data ()->hash = QString(QCryptographicHash::hash (str.toUtf8 (), QCryptographicHash::Md5).toHex ());
}


}
