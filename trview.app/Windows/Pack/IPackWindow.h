#pragma once

#include <functional>
#include <trview.common/Event.h>
#include <trlevel/IPack.h>

namespace trview
{
    struct IPackWindow
    {
        using Source = std::function<std::shared_ptr<IPackWindow>()>;

        virtual ~IPackWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;

        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
        Event<std::string> on_level_open;
    };
}
