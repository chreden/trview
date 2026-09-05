export module trview.app:IWindow;

import std;
import std.compat;

import trview.common;

namespace trview
{
    export struct IWindow : public IRecipient
    {
        using Source = std::function<std::shared_ptr<IWindow>()>;

        virtual ~IWindow() = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual std::string title() const = 0;
        virtual std::string type() const = 0;

        Event<> on_window_closed;
    };
}