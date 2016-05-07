#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QAtomicInt>
#include <QAtomicPointer>
#include <QMutex>
#include <QWaitCondition>

#include <QDebug>

#include "Buffer.h"

namespace PhoenixPlayer {

namespace PlayBackend {

namespace PhoenixBackend {

class RingBuffer
{
public:
    RingBuffer(uint bufferSize, uint ringCount = 8)
        : m_bufferSize(bufferSize)
        , m_capacity(ringCount+1)
        , m_tail(0)
        , m_head(0)
    {
        for (int i=0; i< m_capacity; ++i) {
            m_container.append (new Buffer(bufferSize));
        }
    }

    ~RingBuffer() {
        qDeleteAll(m_container);
        m_container.clear ();
    }

    inline uint bufferSize() const {
        return m_bufferSize;
    }
    // Pop by Consumer can only update the head (load with relaxed, store with release)
    // the tail must be accessed with at least aquire
    bool pop(Buffer *other) {
        const int current_head = m_head.load ();
        const int current_tail = m_tail.loadAcquire ();
        if (current_head ==  current_tail) {
            return false; // empty queue
        }
//        qDebug()<<Q_FUNC_INFO<<" current_head "<<current_head<<" _head "<<current_head
//               <<" current_tail "<<current_tail<<" tail "<<current_tail
//              <<" next head "<<increment (current_head);

        copyBuffers (other, m_container[current_head]);
        m_head.storeRelease (increment (current_head));
        return true;
    }

    bool push(Buffer *other) {
        const int current_tail = m_tail.load ();
        const int next_tail = increment(current_tail);
        const int current_head = m_head.loadAcquire ();
        if (next_tail == current_head) {
            return false; // full queue
        }

//        qDebug()<<Q_FUNC_INFO<<" current_head "<<current_head<<" _head "<<current_head
//               <<" current_tail "<<current_tail<<" tail "<<current_tail
//              <<" next tail "<<next_tail;

        Buffer *b = m_container[current_tail];
        copyBuffers (b, other);
        m_container[current_tail] = b;
        m_tail.storeRelease (next_tail);
        return true;
    }

    bool empty() const {
        // snapshot with acceptance of that this comparison operation is not atomic
        return m_head.load () == m_tail.load ();
    }

    bool full() const {
        const int next_tail = increment (m_tail.load ()); // aquire, we dont know who call
        return next_tail == m_head.load ();
    }
    void clear() {
        m_head.storeRelease (0);
        m_tail.storeRelease (0);
    }

    QMutex *mutex() {
        return &m_mutex;
    }

    QWaitCondition *fullCond() {
        return &m_cnd;
    }
    QWaitCondition *emptyCond() {
        return &m_cnd2;
    }

protected:
    inline bool copyBuffers(Buffer *dest, Buffer *src) {
        if (!dest/* || !dest->data || dest->nbytes == 0*/) {
            qWarning()<<Q_FUNC_INFO<<"No dest buffer found";
            return false;
        }
        if (!src || !src->data || src->nbytes == 0) {
            qWarning()<<Q_FUNC_INFO<<"fill zero to dest buffer";
            memset (dest->data, 0, dest->size);
            dest->nbytes = dest->size;
            return true;
        }
        if (dest->size != src->size) {
            qCritical()<<Q_FUNC_INFO<<"Buffer size not equal";
            return false;
        }
        memmove (dest->data, src->data, src->nbytes);
        dest->nbytes = src->nbytes;
        return true;
    }

private:
    inline int increment(int idx) const {
        return (idx + 1) % m_capacity;
    }
private:
    uint m_bufferSize;
    int m_capacity;
    QAtomicInt m_tail;
    QAtomicInt m_head;
    QList<Buffer *> m_container;
    QMutex m_mutex;
    QWaitCondition m_cnd;
    QWaitCondition m_cnd2;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // RINGBUFFER_H
