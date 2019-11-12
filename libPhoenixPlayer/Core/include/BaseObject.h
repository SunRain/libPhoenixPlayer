#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QObject>
#include <QMetaEnum>
#include <QMetaProperty>
#include <QDebug>

namespace PhoenixPlayer {

#define DECLARE_STATIC_PROPERTY_LIST(Class) \
    public: \
    static QStringList staticPropertyList() { \
        QStringList list; \
        int count = Class::staticMetaObject.propertyCount (); \
        for (int i=0; i<count; ++i) { \
            QMetaProperty prop = Class::staticMetaObject.property (i); \
            const char *name = prop.name (); \
            list.append (name); \
        } \
        return list; \
    } \
    private: \

class BaseObject : public QObject
{
    Q_OBJECT
public:
    BaseObject(QObject *parent = Q_NULLPTR) : QObject(parent) {}
    virtual ~BaseObject() {}

    QString enumToStr(const QString &enumName, int enumValue) const {
        int index  = metaObject()->indexOfEnumerator(enumName.toUtf8());
        QMetaEnum m = metaObject()->enumerator(index);
        return m.valueToKey(enumValue);
    }
    QStringList propertyList() const {
        QStringList list;
        int count = metaObject()->propertyCount();
        for (int i=0; i<count; ++i) {
            QMetaProperty prop = metaObject()->property(i);
            const char *name = prop.name();
            list.append(name);
        }
        return list;
    }
};

}

#endif // BASEOBJECT_H
