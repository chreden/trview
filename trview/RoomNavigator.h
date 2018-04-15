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

    // The room navigator control allows the user to select which room to look at and
    // whether to highlight the current room.
    class RoomNavigator
    {
    public:
        // Create the RoomNavigator control and attach it as a child to the parent control
        // specified.
        // parent: The control to attach the navigator to.
        // texture_storage: The texture storage source to use for any required textures.
        RoomNavigator(ui::Control& parent, const ITextureStorage& texture_storage);

        // Event raised when the user selects a different room.
        // This event is not raised by the set_selected_room function.
        // room: The room that the user has selected.
        Event<uint32_t> on_room_selected;

        // Event raised when the user toggles the highlight mode.
        // This event is not raised by the set_highlight function.
        // highlight: Whether highlighting is enabled or disabled.
        Event<bool> on_highlight;

        // Event raised when the user toggles the alternate mode.
        // flip: Whether the flipmap should be treated as on.
        Event<bool> on_flip;

        // Set the room information for the current room. This will be used to 
        // populate the labels (eg position).
        // room_info: The room information.
        void set_room_info(RoomInfo room_info);

        // Set the maximum number of rooms in the level. This will constrain the
        // room number that the user can select.
        // max_rooms: The number of rooms in the level.
        void set_max_rooms(uint32_t max_rooms);

        // Set the current highlight mode. This will not raise the on_highlight event
        // but will update the user interface appropriately.
        // highlight: Whether the highlight mode is enabled or disabled.
        void set_highlight(bool highlight);

        // Set the selected room in the level. This will not raise the on_room_selected event
        // but will update the user interface appropriately.
        // selected_room: The room that has been selected.
        void set_selected_room(uint32_t selected_room);
    private:
        ui::Checkbox*      _highlight;
        ui::NumericUpDown* _current;
        ui::Label*         _max;
        ui::Label*         _x;
        ui::Label*         _z;
    };
}
