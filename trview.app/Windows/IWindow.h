#pragma once

#include <trview.common/Event.h>
#include <string>
#include <cstdint>

namespace trview
{
    struct IWindow
    {
        using Source = std::function<std::shared_ptr<IWindow>()>;

        virtual ~IWindow() = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual std::string type() const = 0;

        Event<> on_window_closed;
    };
}