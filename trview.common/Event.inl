/// @file Event.inl
/// @brief Class that others can register with to be called back when something happens.
/// 
/// Implementation of the functions defined in Event.h

#pragma once

#include <algorithm>

namespace trview
{
    template <typename... Args>
    EventBase::Token Event<Args...>::operator += (std::function<void(Args...)> listener)
    {
        Token token(this);
        _listeners.emplace_back(&token, listener);
        return token;
    }

    template <typename... Args>
    Event<Args...>& Event<Args...>::operator += (Event<Args...>& listener)
    {
        _listener_events.push_back(&listener);
        listener._subscriptions.push_back(this);
        return *this;
    }

    template <typename... Args>
    Event<Args...>& Event<Args...>::operator -= (Event<Args...>& listener)
    {
        std::erase(_listener_events, &listener);
        std::erase(listener._subscriptions, this);
        return *this;
    }

    template <typename... Args>
    void Event<Args...>::operator()(Args... arguments)
    {
        for (const auto& listener : _listeners)
        {
            listener.second(arguments...);
        }
        for (const auto& func : _listener_events)
        {
            (*func)(arguments...);
        }
    }

    template <typename... Args>
    Event<Args...>::Event(Event<Args...>&& other)
    {
        *this = std::move(other);
    }

    template <typename... Args>
    Event<Args...>::~Event()
    {
        remove_from_chain();
        remove_listeners();
    }

    template <typename... Args>
    Event<Args...>& Event<Args...>::operator =(Event<Args...>&& other)
    {
        remove_from_chain();

        _listeners = std::move(other._listeners);
        _listener_events = std::move(other._listener_events);
        _subscriptions = std::move(other._subscriptions);

        for (auto& listener : _listeners)
        {
            listener.first->replace_event(this);
        }

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
        return *this;
    }

    template <typename... Args>
    void Event<Args...>::remove_from_chain()
    {
        for (auto& listener : _listener_events)
        {
            listener->_subscriptions.erase(
                std::remove(listener->_subscriptions.begin(),
                    listener->_subscriptions.end(),
                    this),
                listener->_subscriptions.end());
        }
        _listener_events.clear();

        for (auto& sub : _subscriptions)
        {
            sub->_listener_events.erase(
                std::remove(sub->_listener_events.begin(),
                    sub->_listener_events.end(),
                    this),
                sub->_listener_events.end());
        }
        _subscriptions.clear();
    }

    template <typename... Args>
    void Event<Args...>::remove_listeners()
    {
        for (auto& listener : _listeners)
        {
            listener.first->replace_event(nullptr);
        }
        _listeners.clear();
    }

    template <typename... Args>
    void Event<Args...>::remove_token(Token* token) 
    {
        _listeners.erase(
            std::remove_if(_listeners.begin(), _listeners.end(),
                [token](const auto& l) { return l.first == token; }), _listeners.end());
    }

    template <typename... Args>
    void Event<Args...>::replace_token(Token* old_token, Token* new_token)
    {
        for (auto& listener : _listeners)
        {
            if (listener.first == old_token)
            {
                listener.first = new_token;
            }
        }
    }
}
