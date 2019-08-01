#include "BufferQueue.h"

using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;




BufferQueue::BufferQueue()
{

}

BufferQueue::~BufferQueue()
{
    auto it = this->begin();
    while (it != this->end()) {
        delete *it;
        ++it;
    }
    QQueue::clear();
}

void BufferQueue::initialization(quint32 sampleRate, int channels)
{
    size_t blockSamples = channels * Buffer::BUFFER_PERIOD;
    int bufferCnt = sampleRate * BUFFER_MS_CNT / 1000 / Buffer::BUFFER_PERIOD;
    if(blockSamples == m_blockSamples && bufferCnt == m_bufferCnt) {
        return;
    }
    m_blockSamples = blockSamples;
    m_bufferCnt = bufferCnt;
    if (m_bufferCnt < QUEUE_MIN_SIZE) {
        m_bufferCnt = QUEUE_MIN_SIZE;
    }
}

void BufferQueue::clear()
{
    auto it = this->begin();
    while (it != this->end()) {
        delete *it;
        ++it;
    }
    QQueue::clear();
}
