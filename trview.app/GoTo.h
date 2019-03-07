/// @file GoToRoom.h
/// @brief UI element that allows the user to type in a number to focus on that.
/// 
/// Creates a text box that can be activated with a shortcut. The user can then type in a
/// room or item number and the camera will be focused on that room.

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
        class GroupBox;
        class TextArea;
    }
    
    /// This window presents the user with a box where they can enter the room number
    /// that they want to go to. Then when they press enter, that will be the selected room.
    class GoTo final
    {
    public:
        /// Creates an instance of the GoTo class. This will add UI elements to the control provided.
        /// @param parent The control to which the instance will be added as a child.
        explicit GoTo(ui::Control& parent);

        /// Gets whether the window is currently visible.
        /// @returns True if the window is visible.
        bool visible() const;

        /// Toggle whether the window is visible.
        void toggle_visible();

        /// Event raised when the user selects a new room. The newly selected room is passed as
        /// a parameter when the event is raised.
        Event<uint32_t> on_selected;

        /// Get the name of the type of thing being selected.
        std::wstring name() const;

        /// Set the name of the type of thing that is being gone to.
        void set_name(const std::wstring& name);
    private:
        TokenStore    _token_store;
        ui::Control*  _window;
        ui::GroupBox* _group;
        ui::TextArea* _text_area;
        std::wstring  _name;
    };
}
