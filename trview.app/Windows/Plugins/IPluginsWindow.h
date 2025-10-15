#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <trview.common/Event.h>

namespace trview
{
    struct IPluginsWindow
    {
        using Source = std::function<std::shared_ptr<IPluginsWindow>()>;
        virtual ~IPluginsWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void update(float dt) = 0;

        Event<> on_window_closed;
    };
}
