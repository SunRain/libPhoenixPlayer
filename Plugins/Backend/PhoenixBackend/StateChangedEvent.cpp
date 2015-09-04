#include "StateChangedEvent.h"

namespace PhoenixPlayer {
namespace PlayBackend {
namespace PhoenixBackend {

StateChangedEvent::StateChangedEvent(PlayState currentState, PlayState previousState)
    :QEvent(EVENT_STATE_CHANGED)
{
    m_state = currentState;
    m_prevState = previousState;
}

StateChangedEvent::~StateChangedEvent()
{
}

PlayState StateChangedEvent::currentState() const
{
    return m_state;
}

PlayState StateChangedEvent::previousState() const
{
    return m_prevState;
}

} //PhoenixBackend
} //PlayBackend
} //PhoenixPlayer
