#pragma once

#include <vector>
#include <functional>

namespace trview
{
    template < typename... Args >
    class Event
    {
    public:
        Event<Args...>&
        operator += (std::function<void(Args...)> listener)
        {
            listeners_.push_back(listener);
            return *this;
        }

        void raise(Args... arguments)
        {
            for(auto& func : listeners_)
            {
                func(arguments...);
            } 
        }
    private:
        std::vector<std::function<void(Args...)>> listeners_;
    };

    template < >
    class Event<void>
    {
    public:
        Event<void>&
        operator += (std::function<void()> listener)
        {
            listeners_.push_back(listener);
            return *this;
        }

        void raise()
        {
            for(auto& func : listeners_)
            {
                func();
            }
        }
    private:
        std::vector<std::function<void()>> listeners_;
    };
}
