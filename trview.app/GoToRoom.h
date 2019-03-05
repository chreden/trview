/// @file GoToRoom.h
/// @brief UI element that allows the user to type in a room number to focus on that room.
/// 
/// Creates a text box that can be activated with a shortcut. The user can then type in a
/// room number and the camera will be focused on that room.

#pragma once

#include <cstdint>
#include <string>
#include <trview.common/Event.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class TextArea;
    }
    
    /// This window presents the user with a box where they can enter the room number
    /// that they want to go to. Then when they press enter, that will be the selected room.
    class GoToRoom
    {
    public:
        /// Creates an instance of the GoToRoom class. This will add UI elements to the 
        /// control provided.
        /// @param parent The control to which the instance will be added as a child.
        explicit GoToRoom(ui::Control& parent);

        /// Gets whether the window is currently visible.
        /// @returns True if the window is visible.
        bool visible() const;

        /// Toggle whether the window is visible.
        void toggle_visible();

        /// Event raised when the user selects a new room. The newly selected room is passed as
        /// a parameter when the event is raised.
        Event<uint32_t> room_selected;
    private:
        TokenStore    _token_store;
        ui::Control*  _window;
        ui::TextArea* _text_area;
    };
}
