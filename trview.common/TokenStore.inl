#pragma once

namespace trview
{
    template <typename... Args>
    void TokenStore::add(Event<Args...>& event, const std::function<void(Args...)>& function)
    {
        _tokens.emplace_back(event += function);
    }
}
