#include "stdafx.h"
#include "GoToRoom.h"

#include <trview.ui/Label.h>
#include <trview.ui/GroupBox.h>

namespace trview
{
    namespace
    {
        const float WindowWidth = 80.0f;
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
            15.f,
            TextAlignment::Centre,
            ParagraphAlignment::Centre);

        _label = label.get();
        _window = window.get();

        box->add_child(std::move(label));
        window->add_child(std::move(box));
        parent.add_child(std::move(window));
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
            room_selected(std::stoul(_input));
            toggle_visible();
        }
        else if (key == VK_ESCAPE)
        {
            toggle_visible();
        }
    }
}
