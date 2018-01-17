#pragma once

#include <memory>
#include <cstdint>

#include "Room.h"

#include <trlevel/trtypes.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Label;
        class Window;
    }

    class RoomWindow
    {
    public:
        // Create a new room window as a child of the specified parent control.
        explicit RoomWindow(ui::Control* parent);

        void set_rooms(std::vector<RoomInfo> rooms);
        void toggle_visibility();
        void cycle();
        void cycle_back();
        bool visible() const;
        uint32_t selected_room() const;
        void select_room(uint32_t room);
    private:
        void update_rooms_status();

        bool                                _visible{ true };
        std::vector<RoomInfo>               _rooms;
        uint32_t                            _room_index{ 0u };
        ui::Window*                         _rooms_window;
        ui::Label*                          _rooms_status;
    };
}
