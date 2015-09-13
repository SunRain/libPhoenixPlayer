#ifndef BASEVOLUME_H
#define BASEVOLUME_H

#include <QObject>

//#include "AudioParameters.h"
namespace PhoenixPlayer{
class Buffer;
namespace PlayBackend {

class BaseVolume : public QObject
{
    Q_OBJECT
public:
    explicit BaseVolume(QObject *parent = 0);
    virtual ~BaseVolume();
    virtual void setVolume (int left, int right);
    virtual void setVolume (int volume);
    virtual int leftVolume() const;
    virtual int rightVolume() const;
    virtual int volume() const;
    virtual void setMuted(bool muted = false);
    virtual bool muted() const;
protected:
    virtual int leftScale() const;
    virtual int rightScale() const;
private:
    int m_left;
    int m_right;
    double m_scaleLeft;
    double m_scaleRight;
    bool m_muted;
};
} //namespace PlayBackend
} //namespace PhoenixPlayer
#endif // BASEVOLUME_H
