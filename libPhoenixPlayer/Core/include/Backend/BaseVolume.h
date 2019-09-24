#ifndef BASEVOLUME_H
#define BASEVOLUME_H

#include <QObject>

namespace PhoenixPlayer{

namespace PlayBackend {

class BaseVolume : public QObject
{
    Q_OBJECT
public:
    explicit BaseVolume(QObject *parent = Q_NULLPTR);
    virtual ~BaseVolume();
    virtual void setVolume (int left, int right);
    virtual void setVolume (int volume);
    virtual int leftVolume() const;
    virtual int rightVolume() const;
    virtual int volume() const;
    virtual void setMuted(bool muted = false);
    virtual bool muted() const;
protected:
    virtual qreal leftScale() const;
    virtual qreal rightScale() const;
private:
    int m_left;
    int m_right;
    qreal m_scaleLeft;
    qreal m_scaleRight;
    bool m_muted;
};
} //namespace PlayBackend
} //namespace PhoenixPlayer
#endif // BASEVOLUME_H
