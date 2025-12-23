#pragma once

#include <trview.common/Event.h>
#include "RouteWindow.h"

namespace trview
{
    struct IRouteWindowManager
    {
        virtual ~IRouteWindowManager() = 0;

        /// Event raised when a route is imported.
        Event<> on_route_open;

        Event<> on_route_reload;
        Event<> on_route_save;

        /// Event raised when a route is exported.
        Event<> on_route_save_as;

        /// Event raised when a waypoint is selected.
        Event<std::weak_ptr<IWaypoint>> on_waypoint_selected;

        Event<> on_window_created;

        Event<std::string> on_level_switch;
        Event<> on_new_route;
        Event<> on_new_randomizer_route;
        Event<std::string, std::string> on_level_reordered;

        /// Render all of the route windows.
        virtual void render() = 0;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;

        /// Create a new route window.
        virtual void create_window() = 0;

        virtual void select_waypoint(const std::weak_ptr<IWaypoint>& waypoint) = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual bool is_window_open() const = 0;
    };
}
