#pragma once

#include <vector>
#include "Event.h"

namespace trview
{
    class TokenStore
    {
    public:
        template <typename... Args>
        void add(Event<Args...>& event, const std::function<void(Args...)>& function);
    private:
        std::vector<EventBase::Token> _tokens;
    };
}

#include "TokenStore.inl"
