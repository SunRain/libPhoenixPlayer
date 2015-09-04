#ifndef STATECHANGEDEVENT_H
#define STATECHANGEDEVENT_H

#include <QEvent>
#include "PhoenixBackend_global.h"
namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

#define EVENT_STATE_CHANGED (QEvent::Type(QEvent::User)) /*!< @internal */
#define EVENT_NEXT_TRACK_REQUEST (QEvent::Type(QEvent::User + 1)) /*!< @internal */
#define EVENT_FINISHED (QEvent::Type(QEvent::User + 2)) /*!< @internal */
//#define EVENT_METADATA_CHANGED (QEvent::Type(QEvent::User + 3)) /*!< @internal */
//#define EVENT_STREAM_INFO_CHANGED (QEvent::Type(QEvent::User + 4)) /*!< @internal */

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

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
#endif // STATECHANGEDEVENT_H
