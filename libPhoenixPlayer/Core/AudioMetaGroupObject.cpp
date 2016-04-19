#include "AudioMetaGroupObject.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>

#include "AudioMetaObject.h"

const static QString KEY_NAME("KEY_NAME");
const static QString KEY_LIST("KEY_LIST");
const static QString KEY_IMG_URI("KEY_IMG_URI");
const static QString KEY_HASH("KEY_HASH");

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
    QString ud;
    foreach (AudioMetaObject obj, d.data ()->list) {
        ud.append (obj.hash ());
        list.append (obj.toObject ());
    }
    o.insert (KEY_HASH, d.data ()->hash);
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
    QString ud;
    foreach (AudioMetaObject obj, d.data ()->list) {
        list.append (obj.toMap ());
        ud.append (obj.hash ());
    }
    o.insert (KEY_HASH, d.data ()->hash);
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

QString AudioMetaGroupObject::keyHash()
{
    return KEY_HASH;
}

void AudioMetaGroupObject::calcHash()
{
    QString str;
    str.append (d.data ()->img.toString ());
    str.append (d.data ()->name);
    foreach (AudioMetaObject obj, d.data ()->list) {
        str.append (obj.hash ());
    }
    d.data ()->hash = QString(QCryptographicHash::hash (str.toUtf8 (), QCryptographicHash::Md5).toHex ());
}


}
