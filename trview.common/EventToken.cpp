#include "Event.h"

namespace trview
{
    EventBase::~EventBase()
    {
    }

    EventBase::Token::Token(EventBase* event)
        : _event(event)
    {
    }

    EventBase::Token::Token(Token&& other)
        : _event(std::move(other._event))
    {
        _event->replace_token(&other, this);
    }

    EventBase::Token& EventBase::Token::operator=(Token&& other)
    {
        _event->remove_token(this);
        _event = std::move(other._event);
        _event->replace_token(&other, this);
        return *this;
    }

    EventBase::Token::~Token()
    {
        // Remove this event token from the event.
        if (_event)
        {
            _event->remove_token(this);
        }
    }
}
