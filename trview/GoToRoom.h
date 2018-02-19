#pragma once

#include <cstdint>
#include <string>
#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Label;
    }
    
    // This window presents the user with a box where they can enter the room number
    // that they want to go to. Then when they press enter, that will be the selected
    // room.
    class GoToRoom
    {
    public:
        explicit        GoToRoom(ui::Control* parent);
        bool            visible() const;
        void            toggle_visible();
        void            character(uint16_t character);
        void            input(uint16_t key);
        Event<uint32_t> room_selected;
    private:
        std::wstring _input;
        ui::Control* _window;
        ui::Label*   _label;
    };
}
