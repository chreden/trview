#pragma once

#include <trview.common/Event.h>

namespace trview
{
    struct ILevel;
    struct ITexturesWindow
    {
        using Source = std::function<std::shared_ptr<ITexturesWindow>()>;

        virtual ~ITexturesWindow() = 0;
        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;

        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}
