#ifndef STATECHANGEDEVENT_H
#define STATECHANGEDEVENT_H

#include <QEvent>
#include "PhoenixBackend_global.h"
namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

#define EVENT_STATE_CHANGED         (QEvent::Type(QEvent::User))
#define EVENT_NEXT_TRACK_REQUEST    (QEvent::Type(QEvent::User + 1))
#define EVENT_FINISHED              (QEvent::Type(QEvent::User + 2))
#define EVENT_SEEK_REQUEST          (QEvent::Type(QEvent::User + 3))


class StateChangedEvent : public QEvent
{
public:
    explicit StateChangedEvent(PlayState currentState, PlayState previousState);
    virtual ~StateChangedEvent();
    PlayState currentState() const;
    PlayState previousState() const;
private:
    PlayState m_state;
    PlayState m_prevState;

};

class SeekEvent : public QEvent
{
public:
    explicit SeekEvent(qint64 tm)
        : QEvent(EVENT_SEEK_REQUEST),
        seekTimeMS(tm)
    {

    }
    ~SeekEvent();
    qint64 seekTimeMS = 0;
};


} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // STATECHANGEDEVENT_H
