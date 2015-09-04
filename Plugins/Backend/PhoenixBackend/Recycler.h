// Copyright (c) 2000-2001 Brad Hughes <bhughes@trolltech.com>
//
// Use, modification and distribution is allowed without limitation,
// warranty, or liability of any kind.
//


#ifndef RECYCLER_H
#define RECYCLER_H

#include <QMutex>
#include <QWaitCondition>
#include "AudioParameters.h"

namespace PhoenixPlayer {
class Buffer;

namespace PlayBackend {

namespace PhoenixBackend {
class Recycler
{
public:
    Recycler();
    ~Recycler();

    ///
    /// rief configure Setups audio parameters of output interface.
    /// \param srate Sample rate.
    /// \param chan Number of channels.
    ///
    void configure(quint32 srate, int chan, AudioParameters::AudioFormat format);

    //Returns true if queue if full, otherwise returns false
    bool full() const;
    bool empty() const;
    int available() const;
    int used() const;

    Buffer *next();

    // Returns current buffer for writing.
    Buffer *get(); // get next in recycle

    void add(); // add to queue
    // Removes current buffer from queue.
    void done(); // add to recycle

    void clear(); // clear queue

    // Returns size of all buffers in bytes.
    unsigned long size() const; // size in bytes

    ///
    /// \brief blockSize
    /// \return block size in bytes.
    ///
    unsigned long blockSize() const; // size in bytes

    QMutex *mutex()
    {
        return &m_mutex;
    }

    QWaitCondition *cond()
    {
        return &m_cnd;
    }
    //Returns  true if the next buffer is used by output. Otherwise returns  false.
    bool blocked();

private:
    unsigned int m_bufferCount;
    unsigned int m_addIndex;
    unsigned int m_doneIndex;
    unsigned int m_currentCount;
    unsigned long m_blockSize;
    Buffer **m_buffers;
    QMutex m_mutex;
    QWaitCondition m_cnd;
    Buffer *m_blocked;
};

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // RECYCLER_H
