#pragma once

#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Elements/IRoom.h"
#include "../Elements/ITrigger.h"
#include "../Routing/IRoute.h"
#include "../Settings/RandomizerSettings.h"

namespace trview
{
    struct IRouteWindow
    {
        using Source = std::function<std::shared_ptr<IRouteWindow>()>;

        virtual ~IRouteWindow() = 0;

        /// Event raised when a waypoint is selected.
        Event<std::weak_ptr<IWaypoint>> on_waypoint_selected;

        /// Event raised when an item is selected.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when a trigger is selected.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when a route file is opened.
        Event<> on_route_open;

        Event<> on_route_reload;
        Event<> on_route_save;

        /// Event raised when a route is exported.
        Event<> on_route_save_as;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        Event<std::string> on_level_switch;

        Event<> on_new_route;
        Event<> on_new_randomizer_route;

        /// Render the window.
        virtual void render() = 0;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;

        /// Select the specified waypoint.
        virtual void select_waypoint(const std::weak_ptr<IWaypoint>& waypoint) = 0;

        /// Set the items to that are in the level.
        /// @param items The items to show.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;

        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) = 0;

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& trigger) = 0;

        virtual void focus() = 0;
        /// <summary>
        /// Update the window.
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
    };
}

