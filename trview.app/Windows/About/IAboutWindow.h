#pragma once

namespace trview
{
    struct IAboutWindow
    {
        using Source = std::function<std::shared_ptr<IAboutWindow>()>;

        Event<> on_window_closed;

        virtual ~IAboutWindow() = 0;
        virtual void focus() = 0;
        virtual void render() = 0;
    };
}
