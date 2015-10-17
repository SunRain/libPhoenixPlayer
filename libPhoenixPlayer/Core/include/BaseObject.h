#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QObject>
#include <QMetaEnum>
#include <QMetaProperty>

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
    BaseObject(QObject *parent = 0) : QObject(parent) {}
    virtual ~BaseObject() {}

    QString enumToStr(const QString &enumName, int enumValue) {
        int index  = metaObject ()->indexOfEnumerator (enumName.toLocal8Bit ());
        QMetaEnum m = metaObject ()->enumerator (index);
        return m.valueToKey (enumValue);
    }
    QStringList propertyList() const {
        QStringList list;
        int count = metaObject ()->propertyCount ();
        for (int i=0; i<count; ++i) {
            QMetaProperty prop = metaObject ()->property (i);
            const char *name = prop.name ();
            list.append (name);
        }
        return list;
    }   
    inline QVariant property(const QString &name) {
        QByteArray qba = name.toLocal8Bit ();
        return property (qba.constData ());
    }
    inline bool setProperty(const QString &name, const QVariant & value) {
        QByteArray qba = name.toLocal8Bit ();
        return setProperty (qba.constData (), value);
    }
};

}

#endif // BASEOBJECT_H
