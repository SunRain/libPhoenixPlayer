#ifndef SINGLETONPOINTER_P_H
#define SINGLETONPOINTER_P_H

#include <QtGlobal>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>
#include <QThreadStorage>
#include <QThread>

namespace CallOnce {
enum ECallOnce {
    CO_Request,
    CO_InProgress,
    CO_Finished
};

Q_GLOBAL_STATIC(QThreadStorage<QAtomicInt*>, once_flag)
}

template <class Function>
inline static void qCallOnce(Function func, QBasicAtomicInt& flag)
{
    using namespace CallOnce;

#if QT_VERSION < 0x050000
    int protectFlag = flag.fetchAndStoreAcquire(flag);
#elif QT_VERSION >= 0x050000
    int protectFlag = flag.fetchAndStoreAcquire(flag.load());
#endif

    if (protectFlag == CO_Finished)
        return;
    if (protectFlag == CO_Request && flag.testAndSetRelaxed(protectFlag,
                                                            CO_InProgress)) {
        func();
        flag.fetchAndStoreRelease(CO_Finished);
    }
    else {
        do {
            QThread::yieldCurrentThread();
        }
        while (!flag.testAndSetAcquire(CO_Finished, CO_Finished));
    }
}

template <class Function>
inline static void qCallOncePerThread(Function func)
{
    using namespace CallOnce;
    if (!once_flag()->hasLocalData()) {
        once_flag()->setLocalData(new QAtomicInt(CO_Request));
        qCallOnce(func, *once_flag()->localData());
    }
}

template <class T>
class Singleton
{
private:
    typedef T* (*CreateInstanceFunction)();
public:
    static T* instance(CreateInstanceFunction create);
private:
    static void init();

    Singleton();
    ~Singleton();
    Q_DISABLE_COPY(Singleton)
    static QBasicAtomicPointer<void> create;
    static QBasicAtomicInt flag;
    static QBasicAtomicPointer<void> tptr;
    bool inited;
};

template <class T>
T* Singleton<T>::instance(CreateInstanceFunction create)
{
    Singleton::create.store(create);
    qCallOnce(init, flag);
    return (T*)tptr.load();
}

template <class T>
void Singleton<T>::init()
{
    static Singleton singleton;
    if (singleton.inited) {
        CreateInstanceFunction createFunction = (CreateInstanceFunction)Singleton::create.load();
        tptr.store(createFunction());
    }
}

template <class T>
Singleton<T>::Singleton() {
    inited = true;
};

template <class T>
Singleton<T>::~Singleton() {
    T* createdTptr = (T*)tptr.fetchAndStoreOrdered(nullptr);
    if (createdTptr) {
        delete createdTptr;
    }
    create.store(nullptr);
}

template<class T> QBasicAtomicPointer<void> Singleton<T>::create = Q_BASIC_ATOMIC_INITIALIZER(nullptr);
template<class T> QBasicAtomicInt Singleton<T>::flag = Q_BASIC_ATOMIC_INITIALIZER(CallOnce::CO_Request);
template<class T> QBasicAtomicPointer<void> Singleton<T>::tptr = Q_BASIC_ATOMIC_INITIALIZER(nullptr);


#endif // SINGLETONPOINTER_P_H

