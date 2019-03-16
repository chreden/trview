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
    namespace ui
    {
        class Control;
        class Checkbox;
        class Label;
        class NumericUpDown;
    }

    struct ITextureStorage;
    struct RoomInfo;

    /// The room navigator control allows the user to select which room to look at and
    /// whether to highlight the current room.
    class RoomNavigator
    {
    public:
        /// Create the RoomNavigator control and attach it as a child to the parent control specified.
        /// @param parent The control to attach the navigator to.
        /// @param texture_storage The texture storage source to use for any required textures.
        RoomNavigator(ui::Control& parent, const ITextureStorage& texture_storage);

        /// Event raised when the user selects a different room. The room that the user has selected is passed as
        /// a parameter when this event is raised.
        /// @remarks This event is not raised by the set_selected_room function.
        Event<int32_t> on_room_selected;

        /// Set the room information for the current room. This will be used to populate the labels (eg position).
        /// @param room_info The room information.
        void set_room_info(RoomInfo room_info);

        /// Set the maximum number of rooms in the level. This will constrain the room number that the user can select.
        /// @param max_rooms The number of rooms in the level.
        void set_max_rooms(uint32_t max_rooms);

        /// Set the selected room in the level. This will not raise the on_room_selected event but will update the user interface appropriately.
        /// @param selected_room The room that has been selected.
        void set_selected_room(uint32_t selected_room);
    private:
        ui::NumericUpDown* _current;
        ui::Label*         _max;
        ui::Label*         _x;
        ui::Label*         _z;
    };
}
