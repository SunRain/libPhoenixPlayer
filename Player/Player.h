#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

namespace PhoenixPlayer {

class Common;
class Settings;

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = 0);
    ~Player();

signals:

public slots:
};

} //PhoenixPlayer
#endif // PLAYER_H
