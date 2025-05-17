#pragma once

#include <trview.common/Event.h>
#include "../Elements/ITrigger.h"

namespace trview
{
    struct IItemsWindow;
    struct PickResult;

    struct IContextMenu
    {
        enum class CopyType
        {
            Position,
            Number
        };

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
        /// Event raised when the user has clicked a copy button.
        /// </summary>
        Event<CopyType> on_copy;
        /// <summary>
        /// Event raised when a trigger has been selected.
        /// </summary>
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<std::weak_ptr<IItemsWindow>> on_filter_items_to_tile;
        virtual void render(const PickResult& pick_result) = 0;
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
        /// Set the triggers that trigger the item in the context menu.
        /// </summary>
        /// <param name="triggers"></param>
        virtual void set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;
        virtual void set_tile_filter_enabled(bool value) = 0;
    };
}
