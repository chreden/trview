#include "stdafx.h"
#include "RoomWindow.h"
#include <vector>
#include <sstream>

#include <trview.ui/Control.h>
#include <trview.ui/Window.h>
#include <trview.ui/Label.h>

namespace trview
{
    RoomWindow::RoomWindow(ui::Control* parent)
    {
        // Create a copy of the room window, to prove that the system works.
        auto room_window = std::make_unique<ui::Window>(
            ui::Point(700, 10),
            ui::Size(256, 256),
            ui::Colour(1.0f, 0.0f, 0.0f, 0.0f));

        room_window->add_child(std::make_unique<ui::Label>(
            ui::Point(0, 0),
            ui::Size(256, 30),
            ui::Colour(1.0f, 0.2f, 0.2f, 0.2f),
            L"Room Window"));

        auto rooms_status = std::make_unique<ui::Label>(
            ui::Point(0, 30),
            ui::Size(256, 226),
            ui::Colour(1.0f, 0.1f, 0.1f, 0.1f),
            L"");

        // Store non owning pointers to manipulate the windows later.
        _rooms_status = rooms_status.get();
        _rooms_window = room_window.get();

        room_window->add_child(std::move(rooms_status));
        parent->add_child(std::move(room_window));
    }

    void RoomWindow::set_rooms(std::vector<RoomInfo> rooms)
    {
        _rooms = rooms;
        _room_index = 0u;
        update_rooms_status();
    }

    void RoomWindow::cycle()
    {
        ++_room_index;
        if (_room_index >= _rooms.size())
        {
            _room_index = 0u;
        }
        update_rooms_status();
    }

    void RoomWindow::cycle_back()
    {
        if (_room_index == 0)
        {
            _room_index = _rooms.size() - 1;
        }
        else
        {
            --_room_index;
        }
        update_rooms_status();
    }

    void RoomWindow::toggle_visibility()
    {
        _rooms_window->set_visible(!visible());
    }

    bool RoomWindow::visible() const
    {
        return _rooms_window->visible();
    }

    uint32_t RoomWindow::selected_room() const
    {
        return _room_index;
    }
    
    void RoomWindow::select_room(uint32_t room)
    {
        if (_room_index < _rooms.size())
        {
            _room_index = room;
            update_rooms_status();
        }
    }

    void RoomWindow::update_rooms_status()
    {
        uint32_t index = selected_room();
        if (_rooms.empty())
        {
            _rooms_status->set_text(L"No room selected");
        }
        else
        {
            auto info = _rooms[index];
            std::wstringstream stream;
            stream << L"Room " << index << L"/" << _rooms.size() - 1 << L": " << info.x / 1024 << L',' << info.z / 1024 << L'\n';
            _rooms_status->set_text(stream.str());
        }
    }
}
