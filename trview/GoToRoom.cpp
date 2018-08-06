#include "stdafx.h"
#include "GoToRoom.h"

#include <trview.ui/Label.h>
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

    GoToRoom::GoToRoom(ui::Control& parent)
    {
        using namespace ui;
        
        auto parent_size = parent.size();

        auto window = std::make_unique<Window>(
            Point(parent_size.width / 2.0f - WindowWidth / 2.0f, parent_size.height / 2.0f - WindowHeight / 2.0f),
            Size(WindowWidth, WindowHeight),
            Colour(1.0f, 0.5f, 0.5f, 0.5f));
        window->set_visible(false);

        auto box = std::make_unique<GroupBox>(
            Point(5, 0),
            Size(WindowWidth - 10, WindowHeight),
            Colour(1.0f, 0.5f, 0.5f, 0.5f),
            Colour(1.0f, 0.0f, 0.0f, 0.0f),
            L"Go to Room");

        auto label = std::make_unique<Label>(
            Point((WindowWidth - 10) / 2.0f - Width / 2.0f,
                  (WindowHeight) / 2.0f - Height / 2.0f),
            Size(Width, Height),
            Colour(1.0f, 0.4f, 0.4f, 0.4f),
            L"",
            8,
            graphics::TextAlignment::Centre,
            graphics::ParagraphAlignment::Centre);

        _label = label.get();
        _window = window.get();

        box->add_child(std::move(label));
        window->add_child(std::move(box));
        parent.add_child(std::move(window));

        _token_store.add(parent.on_size_changed += [&](const Size& size)
        {
            _window->set_position(Point(size.width / 2.0f - _window->size().width / 2.0f, size.height / 2.0f - WindowHeight / 2.0f));
        });
    }

    bool GoToRoom::visible() const
    {
        return _window->visible();
    }

    void GoToRoom::toggle_visible()
    {
        _window->set_visible(!visible());
        if (visible())
        {
            _input.clear();
            _label->set_text(_input);
        }
    }

    void GoToRoom::character(uint16_t character)
    {
        if (!visible())
        {
            return;
        }

        auto value = character - '0';
        if (value >= 0 && value <= 9)
        {
            _input += character;
            _label->set_text(_input);
        }
    }

    void GoToRoom::input(uint16_t key)
    {
        if (!visible())
        {
            return;
        }

        if (key == VK_RETURN)
        {
            try
            {
                auto room = std::stoul(_input);
                room_selected(room);
                toggle_visible();
            }
            catch(...)
            {
                // Couldn't convert the number.
            }
        }
        else if (key == VK_ESCAPE)
        {
            toggle_visible();
        }
    }
}
