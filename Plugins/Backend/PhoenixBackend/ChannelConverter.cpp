#include "ChannelConverter.h"

#include <QList>
#include <QDebug>

#include "Buffer.h"

using namespace PhoenixPlayer::PlayBackend::PhoenixBackend;

ChannelConverter::ChannelConverter(const QList<PhoenixPlayer::AudioParameters::ChannelPosition> &outList)
    : m_outList(outList)
{

}

ChannelConverter::~ChannelConverter()
{
    if(m_tmpBuf) {
        delete [] m_tmpBuf;
        m_tmpBuf = nullptr;
    }
}

void ChannelConverter::apply(PhoenixPlayer::Buffer *buffer)
{
    if(m_disabled) {
        return;
    }

    int inChannels = m_inList.count();
    int outChannels = m_outList.count();

    if(buffer->samples > m_tmpSize) {
        delete [] m_tmpBuf;
        m_tmpBuf = new float[buffer->samples];
        m_tmpSize = buffer->samples;
    }
    memcpy(m_tmpBuf, buffer->data, buffer->samples * sizeof(float));

    size_t samples = buffer->samples * outChannels / inChannels;
    if(samples > buffer->size) {
        delete [] buffer->data;
        buffer->data = new float[samples];
        buffer->size = samples;
    }

    float *in = m_tmpBuf;
    float *out = buffer->data;

    for(unsigned long i = 0; i < buffer->samples / inChannels; ++i) {
        for(int j = 0; j < outChannels; ++j) {
            out[j] = m_reorderArray[j] < 0 ? 0 : in[m_reorderArray[j]];
        }
        in += inChannels;
        out += outChannels;
    }

    buffer->samples = samples;
}

void ChannelConverter::initialization(quint32 sampleRate, const QList<PhoenixPlayer::AudioParameters::ChannelPosition> &in)
{
    AudioEffect::initialization(sampleRate, in);
    m_disabled = (in == m_outList) || (in.count() == 1 && m_outList.count() == 1);
    if (m_disabled) {
        return;
    }

    m_inList = in;
    m_tmpBuf = new float[Buffer::BUFFER_PERIOD * in.count()];
    m_tmpSize = Buffer::BUFFER_PERIOD * in.count();

    QStringList reorderStringList;
    for(int i = 0; i < m_outList.count(); ++i) {
        m_reorderArray[i] = m_outList.indexOf(in.at(i % in.count()));
        reorderStringList << QString("%1").arg(m_reorderArray[i]);
    }

    qDebug()<<Q_FUNC_INFO<<"ChannelConverter: {"<<in
             <<"} ==> {"
             <<m_outList
             <<"}; {"
             <<reorderStringList<<"}";

}
