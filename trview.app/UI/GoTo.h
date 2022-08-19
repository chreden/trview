/// @file GoToRoom.h
/// @brief UI element that allows the user to type in a number to focus on that.
/// 
/// Creates a text box that can be activated with a shortcut. The user can then type in a
/// room or item number and the camera will be focused on that room.

#pragma once

#include <cstdint>
#include <string>
#include <trview.common/Event.h>

namespace trview
{
    /// This window presents the user with a box where they can enter the number of the thing
    /// that they want to go to. Then when they press enter, that will be the selected.
    class GoTo final
    {
    public:
        /// Gets whether the window is currently visible.
        /// @returns True if the window is visible.
        bool visible() const;

        /// Toggle whether the window is visible.
        void toggle_visible(int32_t value);

        /// Event raised when the user selects a new room. The newly selected room is passed as
        /// a parameter when the event is raised.
        Event<uint32_t> on_selected;

        /// Get the name of the type of thing being selected.
        std::string name() const;

        void render();

        /// Set the name of the type of thing that is being gone to.
        void set_name(const std::string& name);
    private:
        std::string  _name;
        bool _visible{ false };
        int _index{ 0 };
        bool _shown{ false };
    };
}
