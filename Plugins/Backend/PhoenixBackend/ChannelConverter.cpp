#include "ChannelConverter.h"

#include <QList>
#include <QDebug>

#include "Buffer.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

ChannelConverter::ChannelConverter(const ChannelMap &out_map)
    : m_out_map(out_map)
{

}

ChannelConverter::~ChannelConverter()
{
    if(m_tmpBuf) {
        delete [] m_tmpBuf;
        m_tmpBuf = nullptr;
    }
}

void ChannelConverter::apply(Buffer *buffer)
{
    if(m_disabled) {
        return;
    }

    qDebug()<<Q_FUNC_INFO<<"Do ChannelConverter.";

    int inChannels = m_in_map.count();
    int outChannels = m_out_map.count();

    if (buffer->samples > m_tmpSize) {
        delete [] m_tmpBuf;
        m_tmpBuf = new float[buffer->samples];
        m_tmpSize = buffer->samples;
    }
    memcpy(m_tmpBuf, buffer->data, buffer->samples * sizeof(float));

    size_t samples = buffer->samples * outChannels / inChannels;
    if (samples > buffer->size) {
        delete [] buffer->data;
        buffer->data = new float[samples];
        buffer->size = samples;
    }

    float *in = m_tmpBuf;
    float *out = buffer->data;

    for (unsigned long i = 0; i < buffer->samples / inChannels; ++i) {
        for(int j = 0; j < outChannels; ++j) {
            out[j] = m_reorderArray[j] < 0 ? 0 : in[m_reorderArray[j]];
        }
        in += inChannels;
        out += outChannels;
    }

    buffer->samples = samples;
}

void ChannelConverter::initialization(quint32 sampleRate, const ChannelMap &in_map)
{
    AudioEffect::initialization(sampleRate, m_out_map);

    if((m_disabled = (in_map == m_out_map)))
        return;

    if((m_disabled = (in_map.count() == 1 && m_out_map.count() == 1)))
        return;

    m_in_map = in_map;
    m_tmpBuf = new float[QMMP_BLOCK_FRAMES * in_map.count()];
    m_tmpSize = QMMP_BLOCK_FRAMES * in_map.count();

    QStringList reorderStringList;
    for(int i = 0; i < m_out_map.count(); ++i) {
        m_reorderArray[i] = m_out_map.indexOf(in_map.at(i % in_map.count()));
        reorderStringList << QString("%1").arg(m_reorderArray[i]);
    }
    qDebug()<<Q_FUNC_INFO<<"ChannelConverter: {"<<in_map.toString()
             <<"} ==> {"
             <<m_out_map.toString()
             <<"}; {"
             <<reorderStringList<<"}";
}

//void ChannelConverter::initialization(quint32 sampleRate, const QList<PhoenixPlayer::AudioParameters::ChannelPosition> &in)
//{
//    AudioEffect::initialization(sampleRate, m_outList);

//    m_disabled = (in == m_outList) || (in.count() == 1 && m_outList.count() == 1);
//    if (m_disabled) {
//        return;
//    }

//    m_inList = in;
//    m_tmpBuf = new float[Buffer::BUFFER_PERIOD * in.count()];
//    m_tmpSize = Buffer::BUFFER_PERIOD * in.count();

//    QStringList reorderStringList;
//    for(int i = 0; i < m_outList.count(); ++i) {
//        m_reorderArray[i] = m_outList.indexOf(in.at(i % in.count()));
//        reorderStringList << QString("%1").arg(m_reorderArray[i]);
//    }

//    qDebug()<<Q_FUNC_INFO<<"ChannelConverter: {"<<in
//             <<"} ==> {"
//             <<m_outList
//             <<"}; {"
//             <<reorderStringList<<"}";

//}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
