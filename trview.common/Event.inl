#pragma once

#include <algorithm>

namespace trview
{
    template <typename... Args>
    Event<Args...>& Event<Args...>::operator += (std::function<void(Args...)> listener)
    {
        _listeners.push_back(listener);
        return *this;
    }

    template <typename... Args>
    Event<Args...>& Event<Args...>::operator += (Event<Args...>& listener)
    {
        _listener_events.push_back(&listener);
        listener._subscriptions.push_back(this);
        return *this;
    }

    template <typename... Args>
    void Event<Args...>::operator()(Args... arguments)
    {
        for (const auto& func : _listeners)
        {
            func(arguments...);
        }
        for (const auto& func : _listener_events)
        {
            (*func)(arguments...);
        }
    }

    template <typename... Args>
    Event<Args...>::Event(Event<Args...>&& other)
    {
        _listeners = std::move(other._listeners);
        _listener_events = std::move(other._listener_events);
        _subscriptions = std::move(other._subscriptions);

        for (auto& listener : _listener_events)
        {
            std::replace(listener->_subscriptions.begin(),
                listener->_subscriptions.end(),
                &other,
                this);
        }

        for (auto& sub : _subscriptions)
        {
            std::replace(sub->_listener_events.begin(),
                sub->_listener_events.end(),
                &other,
                this);
        }
    }

    template <typename... Args>
    Event<Args...>::~Event()
    {
        for (auto& listener : _listener_events)
        {
            listener->_subscriptions.erase(
                std::remove(listener->_subscriptions.begin(),
                            listener->_subscriptions.end(),
                            this),
                listener->_subscriptions.end());
        }

        for (auto& sub : _subscriptions)
        {
            sub->_listener_events.erase(
                std::remove(sub->_listener_events.begin(),
                            sub->_listener_events.end(),
                            this),
                sub->_listener_events.end());
        }
    }

    inline Event<void>& Event<void>::operator += (Event<void>& listener)
    {
        _listeners.push_back([&]()
        {
            listener();
        });
        return *this;
    }

    inline Event<void>& Event<void>::operator += (std::function<void()> listener)
    {
        _listeners.push_back(listener);
        return *this;
    }

    inline void Event<void>::operator()()
    {
        for (auto& func : _listeners)
        {
            func();
        }
    }
}
