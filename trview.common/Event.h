#pragma once

#include <vector>
#include <functional>

namespace trview
{
    template < typename... Args >
    class Event
    {
    public:
        /// Add an event as a listener to this event.
        /// @param listener The event that will be raised when the event is raised.
        Event<Args...>& operator += (Event<Args...>& listener);

        /// Add a function as a listener to this event.
        /// @param listener The function that will be called when the event is raised.
        Event<Args...>& operator += (std::function<void(Args...)> listener);

        /// Raise the event with the provided arguments.
        /// @param arguments The arguments to pass to the listeners.
        void operator()(Args... arguments);

        Event<Args...>() = default;

        Event<Args...>(Event<Args...>&& other);

        ~Event();
    private:
        std::vector<std::function<void(Args...)>> _listeners;
        std::vector<Event<Args...>*> _listener_events;
        std::vector<Event<Args...>*> _subscriptions;
    };
}

#include "Event.inl"
