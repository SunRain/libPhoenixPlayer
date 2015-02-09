#ifndef SINGLETONPOINTER_H
#define SINGLETONPOINTER_H

#include <QDebug>
#include <QMutex>
#include <QScopedPointer>

namespace PhoenixPlayer {

template <class T>
class SingletonPointer
{
public:
//    SingletonPointer(void)
//    {
//        qDebug()<<">>>>>>>> SingletonPointer <<<<<<<<<<<<<";
//        if (m_pInstance == nullptr)
//        {
//            try
//            {
//                m_pInstance = new T;
//                qDebug()<<">>>>>>>> SingletonPointer new instance "
//                       <<m_pInstance->metaObject()->className();
//            }
//            catch (...) //防止new分配内存可能出错的问题，内存分配错误异常为std::bad_alloc
//            {
//                m_pInstance = nullptr;
//            }
//        }

//        m_uiReference++;

//        qDebug()<<"SingletonPointer for "
//                  <<m_pInstance->metaObject()->className()
//                    <<" with num "<<m_uiReference;
//    }

//    ~SingletonPointer(void)
//    {
//        qDebug()<<">>>>>>>> ~ SingletonPointer <<<<<<<<<<<<<";
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

public:
    static T *instance() /*const throw()*/
    {
        qDebug()<<">>>>>>>> SingletonPointer getInstance <<<<<<<<<<<<<";
//        if (m_pInstance == nullptr)
//        {
//            try
//            {
//                m_pInstance = new T;
//                qDebug()<<">>>>>>>> SingletonPointer new instance "
//                       <<m_pInstance->metaObject()->className();
//            }
//            catch (...) //防止new分配内存可能出错的问题，内存分配错误异常为std::bad_alloc
//            {
//                m_pInstance = nullptr;
//            }
//        }

//        m_uiReference++;

//        qDebug()<<"SingletonPointer for "
//                  <<m_pInstance->metaObject()->className()
//                    <<" with num "<<m_uiReference;


//        return m_pInstance;

        if (Q_UNLIKELY(!tptr)) {
            qDebug()<<">>>>>>>> SingletonPointer statr new";
            mLock.lock();
            if (!tptr) {
                qDebug()<<">>>>>>>> SingletonPointer aftre lock , statr new";
                tptr.reset(new T);
            }
            qDebug()<<">>>>>>>> SingletonPointer finish start new, unlock";
            mLock.unlock();
        }
        qDebug()<<">>>>>>>> SingletonPointer return "<<tptr.data()->metaObject()->className();;
        return tptr.data();
    }

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

private:
//    static T *m_pInstance;
//    static  std::size_t m_uiReference;
    static QMutex mLock;
    static QScopedPointer<T> tptr;
//    static T *tp;
};

template<class T>
QScopedPointer<T> SingletonPointer<T>::tptr(0);

template<class T>
QMutex SingletonPointer<T>::mLock(QMutex::NonRecursive);

//template <typename T>
//T *SingletonPointer<T>::tp = 0;

//template <typename T>
//std::size_t SingletonPointer<T>::m_uiReference = 0;

} //PhoenixPlayer

#endif // SINGLETONPOINTER_H
