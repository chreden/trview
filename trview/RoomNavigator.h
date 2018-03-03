#pragma once

#include <cstdint>
#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Button;
        class Label;
        class NumericUpDown;
    }

    struct ITextureStorage;
    class RoomInfo;

    // The room navigator control allows the user to select which room to look at.
    class RoomNavigator
    {
    public:
        RoomNavigator(ui::Control& parent, const ITextureStorage& texture_storage);

        Event<uint32_t> on_room_selected;
        Event<bool> on_highlight;

        void set_room_info(RoomInfo room_info);
        void set_max_rooms(uint32_t max_rooms);
        void set_highlight(bool highlighted);
        void set_selected_room(uint32_t selected_room);
    private:
        ui::Button*        _highlight;
        ui::NumericUpDown* _current;
        ui::Label*         _max;
        ui::Label*         _x;
        ui::Label*         _z;
    };
}
