#pragma once

#include <trview.common/Event.h>
#include "RouteWindow.h"

namespace trview
{
    struct IRouteWindowManager
    {
        virtual ~IRouteWindowManager() = 0;

        /// Event raised when the route colour has changed.
        Event<Colour> on_colour_changed;

        /// Event raised when a route is imported.
        Event<> on_route_open;

        Event<> on_route_reload;
        Event<> on_route_save;

        /// Event raised when a route is exported.
        Event<> on_route_save_as;

        /// <summary>
        /// Event raised when the route stick colour has changed.
        /// </summary>
        Event<Colour> on_waypoint_colour_changed;

        /// Event raised when a waypoint is selected.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when an item is selected.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when a trigger is selected.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when a waypoint is deleted.
        Event<uint32_t> on_waypoint_deleted;

        Event<> on_waypoint_changed;

        /// <summary>
        /// Event raised when a waypoint has moved from one index to another.
        /// </summary>
        Event<int32_t, int32_t> on_waypoint_reordered;

        Event<> on_window_created;

        Event<std::string> on_level_switch;

        /// Render all of the route windows.
        virtual void render() = 0;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;

        /// Create a new route window.
        virtual void create_window() = 0;

        /// Set the items to that are in the level.
        /// @param items The items to show.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;

        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) = 0;

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;

        virtual void select_waypoint(uint32_t index) = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;
        /// <summary>
        /// Set whether randomizer integration is enabled.
        /// </summary>
        /// <param name="value">Whether randomizer integration is enabled.</param>
        virtual void set_randomizer_enabled(bool value) = 0;
        /// <summary>
        /// Set the current randomizer settings.
        /// </summary>
        /// <param name="settings">The settings.</param>
        virtual void set_randomizer_settings(const RandomizerSettings& settings) = 0;

        virtual bool is_window_open() const = 0;
    };
}
