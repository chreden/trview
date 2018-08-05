/// @file Event.h
/// @brief Class that others can register with to be called back when something happens.
/// 
/// Events can be raised which will then call all the functions and events that are
/// registered to listen to this event. Events can be chained together. Subscription
/// is done by the += operator.

#pragma once

#include <vector>
#include <functional>

namespace trview
{
    class EventBase
    {
    public:
        class Token
        {
        public:
            /// Create
            explicit Token(EventBase* event);
            /// Move
            Token(Token&& token);
            /// Move assign.
            Token& operator=(Token&& other);
            /// Destructor.
            ~Token();
        private:
            EventBase* _event;
        };
    protected:
        virtual ~EventBase() = 0;
        virtual void remove_token(Token* token) = 0;
        virtual void replace_token(Token* old_token, Token* new_token) = 0;
    };

    /// Class that others can register with to be called back when something happens.
    template < typename... Args >
    class Event : public EventBase
    {
    public:
        /// Add an event as a listener to this event.
        /// @param listener The event that will be raised when the event is raised.
        Event<Args...>& operator += (Event<Args...>& listener);

        /// Add a function as a listener to this event.
        /// @param listener The function that will be called when the event is raised.
        Token operator += (std::function<void(Args...)> listener);

        /// Raise the event with the provided arguments.
        /// @param arguments The arguments to pass to the listeners.
        void operator()(Args... arguments);

        /// Default constructor for the Event class.
        Event<Args...>() = default;

        /// Move construct an event. Any listeners that are registered on the old event will
        /// be moved to this event and any events that the old event was subscribed to will be
        /// updated to point to the new event.
        /// @param other The event to move construct from.
        Event<Args...>(Event<Args...>&& other);

        /// Destructor for Event. This will inform all events that are listening to this event
        /// that this event has been deleted and will also remove this event from any events that
        /// is has been subscribed to.
        virtual ~Event();

        /// Move assign an event. Any listeners that are registered on the old event will
        /// be moved to this event and any events that the old event was subscribed to will be
        /// updated to point to the new event. Additionally, before the assignment is done the
        /// listeners to the current event will be removed and the event will be removed from
        /// anything it is subscribed to.
        /// @param other The event to move construct from.
        Event<Args...>& operator =(Event<Args...>&& other);
    protected:
        virtual void remove_token(Token* token) override;
        virtual void replace_token(Token* old_token, Token* new_token) override;
    private:
        void remove_from_chain();

        std::vector<std::pair<Token*, std::function<void(Args...)>>> _listeners;
        std::vector<Event<Args...>*> _listener_events;
        std::vector<Event<Args...>*> _subscriptions;
    };
}

#include "Event.inl"
