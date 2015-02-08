#ifndef SINGLETONPOINTER
#define SINGLETONPOINTER

#include <cstddef>
#include <QDebug>

namespace PhoenixPlayer {

template <typename T>
class SingletonPointer
{
public:
    SingletonPointer(void)
    {
        if (m_pInstance == nullptr)
        {
            try
            {
                m_pInstance = new T();
            }
            catch (...) //防止new分配内存可能出错的问题，内存分配错误异常为std::bad_alloc
            {
                m_pInstance = nullptr;
            }
        }

        m_uiReference++;
    }

    ~SingletonPointer(void)
    {
        m_uiReference--;
        if (m_uiReference == 0)
        {
            if (m_pInstance != nullptr)
            {
                delete m_pInstance;
                m_pInstance = nullptr; //非常重要，不然下次再次建立单例的对象的时候错误
            }
        }
    }

public:
    T *getInstance() const throw()
    {
        return m_pInstance;
    }

    T& operator*() const
    {
        return *m_pInstance;
    }

    T *operator->() const throw()
    {
        return m_pInstance;
    }

    operator T *() const throw()        //转换操作符，转化为具体类的指针类型
    {
        return m_pInstance;
    }

private:
    static T *m_pInstance;
    static  std::size_t m_uiReference;
};

template <typename T>
T *SingletonPointer<T>::m_pInstance = nullptr;

template <typename T>
std::size_t SingletonPointer<T>::m_uiReference = 0;

} //PhoenixPlayer
#endif // SINGLETONPOINTER

