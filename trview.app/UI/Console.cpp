#include "Console.h"

namespace trview
{
    Console::Console(ui::Control& parent)
    {
        _window = parent.add_child(std::make_unique<ui::Window>(Point(), Size(500, 300), Colour(0.5f, 0.0f, 0.0f, 0.0f)));
        _token_store += parent.on_size_changed += [&](auto size)
        {
            _window->set_position(Point(size.width - 500, size.height / 2.0f));
        };
        _window->set_position(Point(parent.size().width - 500, parent.size().height / 2.0f));
        _window->set_visible(false);
    }

    bool Console::visible() const
    {
        return _window->visible();
    }

    void Console::set_visible(bool value)
    {
        _window->set_visible(value);
    }
}