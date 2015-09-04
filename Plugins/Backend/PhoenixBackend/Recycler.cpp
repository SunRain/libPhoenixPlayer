#include "Recycler.h"

#include <QDebug>

#include "AudioParameters.h"
#include "Buffer.h"
#include "PhoenixBackend_global.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {
Recycler::Recycler()
{
    m_addIndex = 0;
    m_doneIndex = 0;
    m_currentCount = 0;
    m_bufferCount = 0;
    m_blocked = 0;
    m_blockSize = 0;
    m_buffers = 0;
}

Recycler::~Recycler()
{
    for (unsigned int i = 0; i < m_bufferCount; i++)
    {
        delete m_buffers[i];
        m_buffers[i] = 0;
    }
    if(m_bufferCount)
        delete [] m_buffers;
    m_blocked = 0;
}

void Recycler::configure(quint32 srate, int chan, AudioParameters::AudioFormat format)
{
    unsigned long block_size = AudioParameters::sampleSize(format) * chan * BUFFER_PERIOD;
    unsigned int buffer_count = srate * BUFFER_MSEC / 1000 / BUFFER_PERIOD;
    if(block_size == m_blockSize && buffer_count == m_bufferCount)
        return;

    qDebug()<<Q_FUNC_INFO<<QString("Configure for sampleRate = [%1], channels = [%2], blockSize = [%3], bufferCount = [%4]")
              .arg (srate).arg (chan).arg (block_size).arg (buffer_count);

    for (unsigned int i = 0; i < m_bufferCount; i++)
    {
        delete m_buffers[i];
        m_buffers[i] = 0;
    }
    if(m_bufferCount)
        delete [] m_buffers;
    m_addIndex = 0;
    m_doneIndex = 0;
    m_currentCount = 0;
    m_blocked = 0;
    m_blockSize = block_size;
    m_bufferCount = buffer_count;


    if (m_bufferCount < 4)
        m_bufferCount = 4;

    m_buffers = new Buffer*[m_bufferCount];

    for (unsigned int i = 0; i < m_bufferCount; i++) {
        m_buffers[i] = new Buffer(m_blockSize);
    }
}

bool Recycler::full() const
{
    return m_currentCount == m_bufferCount;
}

bool Recycler::blocked()
{
    return m_buffers[m_addIndex] == m_blocked;
}


bool Recycler::empty() const
{
    return m_currentCount == 0;
}


int Recycler::available() const
{
    return m_bufferCount - m_currentCount;
}

int Recycler::used() const
{
    return m_currentCount;
}


Buffer *Recycler::get()
{
    if (full())
        return 0;
    return m_buffers[m_addIndex];
}

void Recycler::add()
{
    if(m_buffers[m_addIndex]->nbytes)
    {
        m_addIndex = (m_addIndex + 1) % m_bufferCount;
        m_currentCount++;
    }
}

Buffer *Recycler::next()
{
    if(m_currentCount)
    {
        m_blocked = m_buffers[m_doneIndex];
        return m_blocked;
    }
    return 0;
}

void Recycler::done()
{
    m_blocked = 0;
    if (m_currentCount)
    {
        m_currentCount--;
        m_doneIndex = (m_doneIndex + 1) % m_bufferCount;
    }
}

void Recycler::clear()
{
    m_currentCount = 0;
    m_addIndex = 0;
    m_doneIndex = 0;
}

unsigned long Recycler::size() const
{
    return m_bufferCount * m_blockSize;
}

unsigned long Recycler::blockSize() const
{
    return m_blockSize;
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
