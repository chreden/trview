#include "GoTo.h"

#include <Windows.h>
#include <trview.ui/TextArea.h>
#include <trview.ui/GroupBox.h>

namespace trview
{
    namespace
    {
        const float WindowWidth = 87.0f;
        const float WindowHeight = 50.0f;
        const float Width = 50.0f;
        const float Height = 20.0f;
    }

    GoTo::GoTo(ui::Control& parent)
    {
        using namespace ui;

        auto parent_size = parent.size();

        auto window = std::make_unique<Window>(
            Point(parent_size.width / 2.0f - WindowWidth / 2.0f, parent_size.height / 2.0f - WindowHeight / 2.0f),
            Size(WindowWidth, WindowHeight),
            Colour(0.5f, 0.0f, 0.0f, 0.0f));
        window->set_visible(false);

        auto box = std::make_unique<GroupBox>(
            Point(5, 0),
            Size(WindowWidth - 10, WindowHeight),
            Colour::Transparent,
            Colour::Grey,
            L"Go to Room");

        auto text_area = std::make_unique<TextArea>(
            Point((WindowWidth - 10) / 2.0f - Width / 2.0f, (WindowHeight) / 2.0f - Height / 2.0f + 2),
            Size(Width, Height),
            Colour(1.0f, 0.2f, 0.2f, 0.2f),
            Colour::White,
            graphics::TextAlignment::Centre);
        text_area->set_mode(TextArea::Mode::SingleLine);
        _token_store += text_area->on_escape += [&]() { toggle_visible(); };
        _token_store += text_area->on_enter += [&](const std::wstring& text)
        {
            try
            {
                auto index = std::stoul(text);
                on_selected(index);
                toggle_visible();
            }
            catch (...)
            {
                // Couldn't convert the number.
            }
        };

        _text_area = box->add_child(std::move(text_area));
        _group = window->add_child(std::move(box));
        _window = parent.add_child(std::move(window));

        _token_store += parent.on_size_changed += [&](const Size& size)
        {
            _window->set_position(Point(size.width / 2.0f - _window->size().width / 2.0f, size.height / 2.0f - WindowHeight / 2.0f));
        };
    }

    bool GoTo::visible() const
    {
        return _window->visible();
    }

    void GoTo::toggle_visible()
    {
        _window->set_visible(!visible());
        if (visible())
        {
            _text_area->set_text(L"");
            _text_area->on_focus_requested();
        }
        else
        {
            _text_area->on_focus_clear_requested();
        }
    }

    std::wstring GoTo::name() const
    {
        return _name;
    }

    void GoTo::set_name(const std::wstring& name)
    {
        _name = name;
        _group->set_title(L"Go to " + name);
    }
}
