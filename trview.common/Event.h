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
        operator += (Event<Args...>& listener)
        {
            listeners_.push_back([&](auto args)
            {
                listener(args);
            });
            return *this;
        }

        Event<Args...>&
        operator += (std::function<void(Args...)> listener)
        {
            listeners_.push_back(listener);
            return *this;
        }

        void operator()(Args... arguments)
        {
            for (auto& func : listeners_)
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

        void operator()()
        {
            for (auto& func : listeners_)
            {
                func();
            }
        }
    private:
        std::vector<std::function<void()>> listeners_;
    };
}
