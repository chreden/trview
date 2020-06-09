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

        _input = _window->add_child(std::make_unique<ui::TextArea>(Point(0, 282), Size(500, 18), Colour::Blue, Colour::White));
        _input->set_mode(ui::TextArea::Mode::SingleLine);
    }

    bool Console::visible() const
    {
        return _window->visible();
    }

    void Console::set_visible(bool value)
    {
        if (value == _window->visible())
        {
            return;
        }

        _input->set_text(L"");
        _window->set_visible(value);

        if (value)
        {
            _input->on_focus_requested();
        }
        else
        {   
            _input->on_focus_clear_requested();
        }
    }
}