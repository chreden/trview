#pragma once

#include <trview.ui/StackPanel.h>
#include <trview.ui/Button.h>

namespace trview
{
    class ContextMenu final
    {
    public:
        /// Control names used for automation
        struct Names
        {
            static const std::string add_waypoint_button;
            static const std::string add_mid_waypoint_button;
            static const std::string hide_button;
            static const std::string orbit_button;
            static const std::string remove_waypoint_button;
        };

        /// Create a new ContentMenu window.
        /// @param parent The control to add the window to.
        explicit ContextMenu(ui::Control& parent);

        /// Get the root control.
        /// @returns The root control.
        const ui::Control* control() const;

        /// Set the position of the context menu.
        /// @param position The new position of the menu.
        void set_position(const Point& position);

        /// Set the context menu to visible.
        /// @param value The new visibility status.
        void set_visible(bool value);

        /// Set whether the remove waypoint button is enabled or not.
        /// @param value Whether the remove waypoint button is enabled or not.
        void set_remove_enabled(bool value);

        /// Set whether the hide button is enabled or not.
        /// @param value Whether the hide button is enabled or not.
        void set_hide_enabled(bool value);

        /// Get whether the context menu is visible.
        /// @returns Whether the menu is visible.
        bool visible() const;

        /// Event raised when the user has clicked the button to create a new
        /// waypoint for the current route.
        Event<> on_add_waypoint;

        /// Event raised when the user has clicked the button to create a new
        /// mid waypoint for the current route.
        Event<> on_add_mid_waypoint;

        /// Event raised when the user has clicked the remove waypoint button.
        Event<> on_remove_waypoint;

        /// Event raised when the user has clicked the orbit here button.
        Event<> on_orbit_here;

        /// Event raised when the user has clicked the hide button.
        Event<> on_hide;
    private:
        ui::StackPanel* _menu;
        ui::Button*     _remove_button;
        ui::Button*     _hide_button;
        bool            _remove_enabled{ false };
        bool            _hide_enabled{ false };
        TokenStore      _token_store;
    };
}
