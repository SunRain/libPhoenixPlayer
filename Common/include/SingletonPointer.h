#ifndef SINGLETONPOINTER
#define SINGLETONPOINTER

#include <cstddef>
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

Q_GLOBAL_STATIC(QThreadStorage<QAtomicInt*>, once_flag)

template <class Function>
inline static void qCallOnce(Function func, QBasicAtomicInt& flag)
{
    using namespace CallOnce;

#if QT_VERSION < 0x050000
    int protectFlag = flag.fetchAndStoreAcquire(flag);
#endif
#if QT_VERSION >= 0x050000
    int protectFlag = flag.fetchAndStoreAcquire(flag.load());
#endif

   qDebug()<<"protectFlag is "<<protectFlag;

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
class SingletonPointer
{
public:
//    static T& instance()
//    {
//        qCallOnce(init, flag);
//        return *tptr;
//    }
    static T* instance()
    {
        qCallOnce(init, flag);
        return tptr.data ();
    }
    static void init()
    {
        qDebug()<<"SingletonPointer init";
        tptr.reset(new T);
    }

private:
    SingletonPointer() {

    }
    ~SingletonPointer() {

    }
    Q_DISABLE_COPY(SingletonPointer)

    static QScopedPointer<T> tptr;
    static QBasicAtomicInt flag;
};

template<class T> QScopedPointer<T> SingletonPointer<T>::tptr(0);
template<class T> QBasicAtomicInt SingletonPointer<T>::flag
= Q_BASIC_ATOMIC_INITIALIZER(CallOnce::CO_Request);


//template <typename T>
//class SingletonPointer
//{
//public:
//    SingletonPointer(void)
//    {
//        if (m_pInstance == nullptr)
//        {
//            try
//            {
//                m_pInstance = new T();
//            }
//            catch (...) //防止new分配内存可能出错的问题，内存分配错误异常为std::bad_alloc
//            {
//                m_pInstance = nullptr;
//            }
//        }

//        m_uiReference++;
//    }

//    ~SingletonPointer(void)
//    {
//        m_uiReference--;
//        if (m_uiReference == 0)
//        {
//            if (m_pInstance != nullptr)
//            {
//                delete m_pInstance;
//                m_pInstance = nullptr; //非常重要，不然下次再次建立单例的对象的时候错误
//            }
//        }
//    }

//public:
//    T *getInstance() const throw()
//    {
//        return m_pInstance;
//    }

//    T& operator*() const
//    {
//        return *m_pInstance;
//    }

//    T *operator->() const throw()
//    {
//        return m_pInstance;
//    }

//    operator T *() const throw()        //转换操作符，转化为具体类的指针类型
//    {
//        return m_pInstance;
//    }

//private:
//    static T *m_pInstance;
//    static  std::size_t m_uiReference;
//};

//template <typename T>
//T *SingletonPointer<T>::m_pInstance = nullptr;

//template <typename T>
//std::size_t SingletonPointer<T>::m_uiReference = 0;

} //PhoenixPlayer
#endif // SINGLETONPOINTER

