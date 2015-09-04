#ifndef BASEVISUAL_H
#define BASEVISUAL_H

#include <QMutex>
#include <QObject>

namespace PhoenixPlayer{
namespace PlayBackend {

class BaseVisual : public QObject
{
    Q_OBJECT
public:
    explicit BaseVisual(QObject *parent);
    virtual ~BaseVisual() {}

    ///
    /// \brief add Adds data for visualization.
    /// \param data Audio data.
    /// \param size Size of audio data.
    /// \param chan Number of channels.
    ///
    virtual void add(unsigned char *data, qint64 size, int chan) = 0;

    ///
    /// \brief clear Resets visual buffers.
    ///
    virtual void clear() = 0;

    QMutex *mutex() {
        return &m_mutex;
    }

private:
    QMutex m_mutex;
};


} //PlayBackend
} //PhoenixPlayer
#endif // BASEVISUAL_H
