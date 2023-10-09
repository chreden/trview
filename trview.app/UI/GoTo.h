/// @file GoToRoom.h
/// @brief UI element that allows the user to type in a number to focus on that.
/// 
/// Creates a text box that can be activated with a shortcut. The user can then type in a
/// room or item number and the camera will be focused on that room.

#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <trview.common/Event.h>

namespace trview
{
    struct IItem;
    struct IRoom;

    /// This window presents the user with a box where they can enter the number of the thing
    /// that they want to go to. Then when they press enter, that will be the selected.
    class GoTo final
    {
    public:
        struct GoToItem final
        {
            uint32_t    number;
            std::string name;
            std::variant<std::weak_ptr<IItem>, std::weak_ptr<IRoom>> item;
        };

        /// Gets whether the window is currently visible.
        /// @returns True if the window is visible.
        bool visible() const;

        /// Toggle whether the window is visible.
        void toggle_visible();

        /// Event raised when the user selects a new room. The newly selected room is passed as
        /// a parameter when the event is raised.
        Event<GoToItem> on_selected;

        void render();

        void set_items(const std::vector<GoToItem>& items);
    private:
        bool _visible{ false };
        bool _shown{ false };
        std::vector<GoToItem> _items;
        std::string _current_input;
        bool _need_focus{ false };
    };
}
