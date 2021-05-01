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
        Event<std::string> on_route_import;

        /// Event raised when a route is exported.
        Event<std::string> on_route_export;

        /// Event raised when a waypoint is selected.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when an item is selected.
        Event<Item> on_item_selected;

        /// Event raised when a trigger is selected.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when a waypoint is deleted.
        Event<uint32_t> on_waypoint_deleted;

        /// Render all of the route windows.
        /// @param vsync Whether to use vsync.
        virtual void render(bool vsync) = 0;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(IRoute* route) = 0;

        /// Create a new route window.
        virtual void create_window() = 0;

        /// Set the items to that are in the level.
        /// @param items The items to show.
        virtual void set_items(const std::vector<Item>& items) = 0;

        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) = 0;

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) = 0;

        virtual void select_waypoint(uint32_t index) = 0;
    };
}
