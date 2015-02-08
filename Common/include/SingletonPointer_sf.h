#ifndef SINGLETONPOINTER_H
#define SINGLETONPOINTER_H

#include <QDebug>

#include <QtCore/QtGlobal>
#include <QtCore/QAtomicInt>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QThreadStorage>
#include <QtCore/QThread>
#include <QtCore/QtGlobal>
#include <QtCore/QScopedPointer>

namespace PhoenixPlayer {

namespace CallOnce {
enum ECallOnce {
    CO_Request,
    CO_InProgress,
    CO_Finished
};
}

template <class Function>
inline static void qCallOnce_sf(Function func, int& flag)
{
    using namespace CallOnce;

   qDebug()<<"SF protectFlag is "<<flag;

    if (flag == (int)CO_Finished)
        return;
    if (flag == (int)CO_Request) {
        qDebug()<<"SF init";
        func();
        flag = (int)CO_Finished;
    }
    else {
        do {
            QThread::yieldCurrentThread();
        }
        while (!(flag == (int)CO_Finished));
    }
}

template <class T>
class SingletonPointer
{
public:
    static T* instance()
    {
        qCallOnce_sf(init, flag);
        return tptr.data ();
    }
    static void init()
    {
        qDebug()<<"SF SingletonPointer init";
        tptr.reset(new T);
    }

private:
    SingletonPointer() {

    }
    ~SingletonPointer() {

    }
    Q_DISABLE_COPY(SingletonPointer)

    static QScopedPointer<T> tptr;
    static int flag;
};

template<class T> QScopedPointer<T> SingletonPointer<T>::tptr(0);
template<class T> int SingletonPointer<T>::flag = (int)CallOnce::CO_Request;

} //PhoenixPlayer

#endif // SINGLETONPOINTER_H
