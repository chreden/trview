#pragma once

namespace trview
{
    struct ILogWindow
    {
        using Source = std::function<std::shared_ptr<ILogWindow>()>;

        virtual ~ILogWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}