#pragma once

#include <trview.common/Event.h>

namespace trview
{
    struct IDiffWindow
    {
        using Source = std::function<std::shared_ptr<IDiffWindow>()>;

        virtual ~IDiffWindow() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void set_number(int32_t number) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}