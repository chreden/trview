#pragma once

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

        // Tell listener events that we have moved.
        for (auto& listener : _listener_events)
        {
            for (std::size_t i = 0; i < listener->_subscriptions.size(); ++i)
            {
                if (listener->_subscriptions[i] == &other)
                {
                    listener->_subscriptions[i] = this;
                }
            }
        }

        // Tell subscriptions that we have moved
        for (auto& sub : _subscriptions)
        {
            for (std::size_t i = 0; i < sub->_listener_events.size(); ++i)
            {
                if (sub->_listener_events[i] == &other)
                {
                    sub->_listener_events[i] = this;
                }
            }
        }
    }

    template <typename... Args>
    Event<Args...>::~Event()
    {
        // Tell up and downstream we're gone.
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
