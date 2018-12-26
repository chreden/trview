#pragma once

#include <trview.ui/StackPanel.h>
#include <trview.ui/Label.h>

namespace trview
{
    class ContextMenu final
    {
    public:
        /// Create a new ContentMenu window.
        /// @param parent The control to add the window to.
        explicit ContextMenu(ui::Control& parent);

        /// Set the position of the context menu.
        /// @param position The new position of the menu.
        void set_position(const Point& position);

        /// Set the context menu to visible.
        /// @param value The new visibility status.
        void set_visible(bool value);

        /// Set whether the remove waypoint button is enabled or not.
        /// @param value Whether the remove waypoint button is enabled or not.
        void set_remove_enabled(bool value);

        /// Event raised when the user has clicked the button to create a new
        /// waypoint for the current route.
        Event<> on_add_waypoint;

        /// Event raised when the user has clicked the remove waypoint button.
        Event<> on_remove_waypoint;
    private:
        ui::StackPanel* _menu;
        ui::Label*      _remove_label;
    };
}
