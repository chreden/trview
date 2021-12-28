#pragma once

#include <memory>
#include <functional>
#include <trview.common/Event.h>

namespace trview
{
    class Window;
    struct ILauWindow
    {
        using Source = std::function<std::shared_ptr<ILauWindow>(const Window&)>;

        Event<> on_window_closed;

        virtual ~ILauWindow() = 0;
        virtual void render(bool vsync) = 0;
    };
}
