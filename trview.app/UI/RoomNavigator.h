/// @file RoomNavigator.h
/// @brief UI element that allows the user to toggle various room modes and to select different rooms.
/// 
/// Adds a small UI element that has a checkbox for each room mode and also allows the user to control
/// the currently selected room.

#pragma once

#include <cstdint>
#include <trview.common/Event.h>

namespace trview
{
    /// The room navigator control allows the user to select which room to look at and
    /// whether to highlight the current room.
    class RoomNavigator final
    {
    public:
        /// Event raised when the user selects a different room. The room that the user has selected is passed as
        /// a parameter when this event is raised.
        /// @remarks This event is not raised by the set_selected_room function.
        Event<int32_t> on_room_selected;

        void render();

        /// Set the maximum number of rooms in the level. This will constrain the room number that the user can select.
        /// @param max_rooms The number of rooms in the level.
        void set_max_rooms(uint32_t max_rooms);

        /// Set the selected room in the level. This will not raise the on_room_selected event but will update the user interface appropriately.
        /// @param selected_room The room that has been selected.
        void set_selected_room(uint32_t selected_room);
    private:
        int32_t _selected_room{ 0 };
        int32_t _max_rooms{ 0 };
    };
}
