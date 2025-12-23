#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <trview.common/Event.h>

namespace trview
{
    struct ISoundsWindow
    {
        using Source = std::function<std::shared_ptr<ISoundsWindow>()>;

        virtual ~ISoundsWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;

        Event<> on_window_closed;
    };
}
