#ifndef PHOENIXPLAYER_H
#define PHOENIXPLAYER_H

#include <QObject>

namespace PhoenixPlayer {

class PhoenixPlayer : public QObject
{
    Q_OBJECT
public:
    explicit PhoenixPlayer(QObject *parent = 0);
    virtual ~PhoenixPlayer();
};

} //PhoenixPlayer
#endif // PHOENIXPLAYER_H
