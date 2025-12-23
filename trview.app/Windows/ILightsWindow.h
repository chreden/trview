#pragma once

#include <trview.common/Event.h>

namespace trview
{
    struct IRoom;

    struct ILightsWindow
    {
        using Source = std::function<std::shared_ptr<ILightsWindow>()>;
        virtual ~ILightsWindow() = 0;
        virtual void clear_selected_light() = 0;
        virtual void render() = 0;
        virtual void update(float delta) = 0;
        virtual void set_number(int32_t number) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}
