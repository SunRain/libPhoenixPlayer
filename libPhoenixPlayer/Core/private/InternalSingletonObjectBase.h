#ifndef INTERNALSINGLETONOBJECTBASE_H
#define INTERNALSINGLETONOBJECTBASE_H

#include <QObject>
#include <QMutex>
#include <QList>

namespace PhoenixPlayer {

class InternalSingletonObjectBase : public QObject
{
    Q_OBJECT
public:
    explicit InternalSingletonObjectBase(QObject *parent = Q_NULLPTR);
    virtual ~InternalSingletonObjectBase();

    inline void registeParent(QObject *parent)
    {
        if (!parent) {
            return;
        }
        m_mutex.lock();
        if (!m_parentList.contains(parent)) {
            m_parentList.append(parent);
        }
        m_mutex.unlock();
    }
    void unRegisteParent(QObject *parent)
    {
        if (!parent) {
            return;
        }
        m_mutex.lock();
        if (m_parentList.contains(parent)) {
            m_parentList.removeOne(parent);
        }
        m_mutex.unlock();
    }

protected:
    inline QMutex *mutex()
    {
        return &m_mutex;
    }

    inline QList<QObject* > *parentList()
    {
        return &m_parentList;
    }

private:
    QList<QObject* > m_parentList;

    QMutex      m_mutex;


};

} // PhoenixPlayer
#endif // INTERNALSINGLETONOBJECTBASE_H
