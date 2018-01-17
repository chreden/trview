#include "stdafx.h"
#include "GoToRoom.h"
#include <sstream>

#include <trview.ui/Label.h>

namespace trview
{
    namespace
    {
        const int Width = 50;
        const int Height = 30;
    }

    GoToRoom::GoToRoom(ui::Control* parent)
    {
        auto parent_size = parent->size();

        auto label = std::make_unique<ui::Label>(
            ui::Point(parent_size.width / 2.0f - Width / 2.0f,
                      parent_size.height / 2.0f - Height / 2.0f),
            ui::Size(Width, Height),
            ui::Colour(1.0f, 0.25f, 0.25f, 0.25f),
            L"",
            15.f,
            ui::TextAlignment::Centre,
            ui::ParagraphAlignment::Centre);

        label->set_visible(false);
        _label = label.get();
        parent->add_child(std::move(label));
    }

    bool GoToRoom::visible() const
    {
        return _label->visible();
    }

    void GoToRoom::toggle_visible()
    {
        _label->set_visible(!visible());
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
            std::wstringstream stream(_input);
            uint32_t room = 0;
            stream >> room;
            room_selected.raise(room);
            toggle_visible();
        }
        else if (key == VK_ESCAPE)
        {
            toggle_visible();
        }
    }
}
