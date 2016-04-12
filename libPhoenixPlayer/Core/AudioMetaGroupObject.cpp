#include "AudioMetaGroupObject.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "AudioMetaObject.h"

const static QString KEY_NAME("KEY_NAME");
const static QString KEY_LIST("KEY_LIST");
const static QString KEY_IMG_URI("KEY_IMG_URI");

namespace PhoenixPlayer {

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
    foreach (AudioMetaObject obj, d.data ()->list) {
        list.append (obj.toObject ());
    }
    o.insert (KEY_LIST, list);
    o.insert (KEY_IMG_URI, d.data ()->img.toString ());
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
    foreach (AudioMetaObject obj, d.data ()->list) {
        list.append (obj.toMap ());
    }
    o.insert (KEY_LIST, list);
    o.insert (KEY_IMG_URI, d.data ()->img);
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


}
