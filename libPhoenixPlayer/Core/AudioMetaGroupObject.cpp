#include "AudioMetaGroupObject.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "AudioMetaObject.h"

const static QString KEY_NAME("KEY_NAME");
const static QString KEY_LIST("KEY_LIST");

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
    QJsonArray list;
    foreach (AudioMetaObject obj, d.data ()->list) {
        list.append (obj.toObject ());
    }
    o.insert (KEY_LIST, list);
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


}
