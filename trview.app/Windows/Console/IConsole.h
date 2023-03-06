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
        virtual void set_font(ImFont* font) = 0;

        Event<> on_window_closed;
    };
}
