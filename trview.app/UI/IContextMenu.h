#pragma once

#include <trview.ui/Control.h>

namespace trview
{
    struct IContextMenu
    {
        using Source = std::function<std::unique_ptr<IContextMenu>(ui::Control&)>;

        virtual ~IContextMenu() = 0;
        /// <summary>
        /// Event raised when the user has clicked the button to create a new waypoint for the current route.
        /// </summary>
        Event<> on_add_waypoint;
        /// <summary>
        /// Event raised when the user has clicked the button to create a new mid waypoint for the current route.
        /// </summary>
        Event<> on_add_mid_waypoint;
        /// <summary>
        /// Event raised when the user has clicked the remove waypoint button.
        /// </summary>
        Event<> on_remove_waypoint;
        /// <summary>
        /// Event raised when the user has clicked the orbit here button.
        /// </summary>
        Event<> on_orbit_here;
        /// <summary>
        /// Event raised when the user has clicked the hide button.
        /// </summary>
        Event<> on_hide;
        /// <summary>
        /// Event raised when the user has clicked the rando secret button.
        /// </summary>
        Event<> on_rando_secret;
        /// <summary>
        /// Set the position of the context menu.
        /// </summary>
        /// <param name="position">The new position of the menu.</param>
        virtual void set_position(const Point& position) = 0;
        /// <summary>
        /// Set the context menu to visible.
        /// </summary>
        /// <param name="value">The new visibility status.</param>
        virtual void set_visible(bool value) = 0;
        /// <summary>
        /// Set whether the remove waypoint button is enabled or not.
        /// </summary>
        /// <param name="value">Whether the remove waypoint button is enabled or not.</param>
        virtual void set_remove_enabled(bool value) = 0;
        /// <summary>
        /// Set whether the hide button is enabled or not.
        /// </summary>
        /// <param name="value">Whether the hide button is enabled or not.</param>
        virtual void set_hide_enabled(bool value) = 0;
        /// <summary>
        /// Get whether the context menu is visible.
        /// </summary>
        /// <returns>Whether the menu is visible.</returns>
        virtual bool visible() const = 0;
        /// <summary>
        /// Set whether the mid waypoint button is enabled.
        /// </summary>
        /// <param name="value">Whether the button is enabled.</param>
        virtual void set_mid_waypoint_enabled(bool value) = 0;
        /// <summary>
        /// Set whether the randomizer tools are enabled.
        /// </summary>
        /// <param name="value">Whether the randomizer buttons are enabled.</param>
        virtual void set_randomizer_tools(bool value) = 0;
    };
}
