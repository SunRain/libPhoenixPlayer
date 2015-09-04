#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QObject>
#include <QMetaEnum>

namespace PhoenixPlayer {

class BaseObject : public QObject
{
    Q_OBJECT
public:
    BaseObject(QObject *parent = 0) : QObject(parent) {}
    virtual ~BaseObject() {}

    QString enumToStr(const QString &enumName, int enumValue) {
        int index  = metaObject ()->indexOfEnumerator (enumName.toLocal8Bit ());
        QMetaEnum m = metaObject ()->enumerator (index);
        return m.valueToKey (enumValue);
    }
};

}

#endif // BASEOBJECT_H
