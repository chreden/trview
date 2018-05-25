#pragma once

namespace trview
{
    template <typename... Args>
    Event<Args...>& Event<Args...>::operator += (std::function<void(Args...)> listener)
    {
        listeners_.push_back(listener);
        return *this;
    }

    template <typename... Args>
    Event<Args...>& Event<Args...>::operator += (Event<Args...>& listener)
    {
        listeners_.push_back([&](auto args)
        {
            listener(args);
        });
        return *this;
    }

    template <typename... Args>
    void Event<Args...>::operator()(Args... arguments)
    {
        for (auto& func : listeners_)
        {
            func(arguments...);
        }
    }

    inline Event<void>& Event<void>::operator += (Event<void>& listener)
    {
        listeners_.push_back([&]()
        {
            listener();
        });
        return *this;
    }

    inline Event<void>& Event<void>::operator += (std::function<void()> listener)
    {
        listeners_.push_back(listener);
        return *this;
    }

    inline void Event<void>::operator()()
    {
        for (auto& func : listeners_)
        {
            func();
        }
    }
}
