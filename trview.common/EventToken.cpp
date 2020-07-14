#include "stdafx.h"
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
        : _event(nullptr)
    {
        *this = std::move(other);
    }

    EventBase::Token& EventBase::Token::operator=(Token&& other)
    {
        if (_event)
        {
            _event->remove_token(this);
        }
        _event = other._event;
        if (_event)
        {
            _event->replace_token(&other, this);
        }
        other._event = nullptr;
        return *this;
    }

    EventBase::Token::~Token()
    {
        if (_event)
        {
            _event->remove_token(this);
        }
    }

    void EventBase::Token::replace_event(EventBase* event)
    {
        _event = event;
    }
}
