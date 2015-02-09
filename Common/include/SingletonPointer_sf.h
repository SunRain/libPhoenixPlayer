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
    SingletonPointer()
    {
        qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
        if (Q_UNLIKELY(tptr.isNull())) {
            qDebug()<<">>>>>>>> SingletonPointer statr new";
            mLock.lock();
            if (tptr.isNull()) {
                qDebug()<<">>>>>>>> SingletonPointer aftre lock , statr new";
                tptr.reset(new T);
            }
            qDebug()<<">>>>>>>> SingletonPointer finish start new, unlock";
            mLock.unlock();
        }
        qDebug()<<">>>>>>>> SingletonPointer return "<<tptr.data()->metaObject()->className();;
    }

    ~SingletonPointer()
    {
        qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
    }

public:
    T *instance()
    {
        qDebug()<<">>>>>>>> SingletonPointer getInstance <<<<<<<<<<<<<";
        return tptr.data();
    }
private:
    Q_DISABLE_COPY(SingletonPointer)
    static QMutex mLock;
    static QScopedPointer<T> tptr;
};

template<class T> QScopedPointer<T> SingletonPointer<T>::tptr(0);
template<class T> QMutex SingletonPointer<T>::mLock(QMutex::NonRecursive);

//template <typename T>
//class SingletonPointer
//{
//public:
//    SingletonPointer(void)
//    {
//        qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
//        if (m_pInstance == nullptr)
//        {
//            qDebug()<<">>>>>>>> SingletonPointer statr new";
//            try
//            {
//                m_pInstance = new T;
//            }
//            catch (...) //防止new分配内存可能出错的问题，内存分配错误异常为std::bad_alloc
//            {
//                m_pInstance = nullptr;
//            }
//        }

//        m_uiReference++;
//        qDebug()<<">>>>>>>> SingletonPointer return "
//               <<m_pInstance->metaObject()->className()
//                 <<" with m_uiReference "<<m_uiReference;
//    }

//    ~SingletonPointer(void)
//    {
//        qDebug()<<">>>>>>>>"<<__FUNCTION__<<"<<<<<<<<<<<<<<";
//        m_uiReference--;
//        qDebug()<<" cur m_uiReference "<<m_uiReference;
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
//    T *instance() const throw()
//    {
//        qDebug()<<">>>>>>>> SingletonPointer getInstance <<<<<<<<<<<<<";
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

#endif // SINGLETONPOINTER_H
