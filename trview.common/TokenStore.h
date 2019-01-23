#pragma once

#include <vector>
#include "Event.h"

namespace trview
{
    class TokenStore
    {
    public:
        TokenStore& operator += (EventBase::Token&& token);
    private:
        std::vector<EventBase::Token> _tokens;
    };
}
