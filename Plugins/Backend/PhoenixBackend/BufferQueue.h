#ifndef BUFFERQUEUE_H
#define BUFFERQUEUE_H

#include <QMutex>
#include <QWaitCondition>
#include <QQueue>

#include "Buffer.h"

namespace PhoenixPlayer {

namespace PlayBackend {

namespace PhoenixBackend {

class BufferQueue : public QQueue<Buffer *>
{
public:
    BufferQueue();
    ~BufferQueue();

    ///
    /// \brief BUFFER_MS_CNT buffer size in milliseconds
    ///
    const static uint BUFFER_MS_CNT = 500;

    ///
    /// \brief QUEUE_MIN_SIZE min buffer count for queue
    ///
    const static int QUEUE_MIN_SIZE = 4;

    void initialization(quint32 sampleRate, int channels);

    inline QMutex *mutex()
    {
        return &m_mutex;
    }

    inline QWaitCondition *waitIn()
    {
        return &m_waitIn;
    }

    inline QWaitCondition *waitOut()
    {
        return &m_waitOut;
    }

    ///
    /// \brief blockSamples block size in samples
    /// \return
    ///
    inline size_t blockSamples() const
    {
        return m_blockSamples;
    }

    ///
    /// \brief samples
    /// \return size of all buffers in samples.
    ///
    inline size_t samples() const
    {
        return m_bufferCnt * m_blockSamples;
    }

    inline int maxAvailableCnt() const
    {
        return m_bufferCnt;
    }

    void clear();

private:
    size_t  m_blockSamples;
    int    m_bufferCnt;

    QMutex          m_mutex;
    QWaitCondition  m_waitIn;
    QWaitCondition  m_waitOut;

};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer

#endif // BUFFERQUEUE_H
