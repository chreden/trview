#pragma once

#include <functional>

namespace trview
{
    struct IConsole
    {
        using Source = std::function<std::shared_ptr<IConsole>()>;

        virtual ~IConsole() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void initialise_ui() = 0;
        virtual void print(const std::string& text) = 0;

        Event<> on_window_closed;
        Event<std::string> on_command;
    };
}
