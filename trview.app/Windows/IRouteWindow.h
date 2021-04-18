#pragma once

#include <trview.common/Event.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Room.h>
#include <trview.app/Elements/Trigger.h>
#include <trview.app/Routing/IRoute.h>

namespace trview
{
    struct IRouteWindow
    {
        using Source = std::function<std::shared_ptr<IRouteWindow>(const Window)>;

        virtual ~IRouteWindow() = 0;

        /// Event raised when a waypoint is selected.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when an item is selected.
        Event<Item> on_item_selected;

        /// Event raised when a trigger is selected.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the route colour has been changed.
        Event<Colour> on_colour_changed;

        /// Event raised when a route file is opened.
        Event<std::string> on_route_import;

        /// Event raised when a route is exported.
        Event<std::string> on_route_export;

        /// Event raised when a waypoint is deleted.
        Event<uint32_t> on_waypoint_deleted;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        /// Render the window.
        /// @param vsync Whether to use vsync or not.
        virtual void render(bool vsync) = 0;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(IRoute* route) = 0;

        /// Select the specified waypoint.
        /// @param index The index of the waypoint to select.
        virtual void select_waypoint(uint32_t index) = 0;

        /// Set the items to that are in the level.
        /// @param items The items to show.
        virtual void set_items(const std::vector<Item>& items) = 0;

        virtual void set_rooms(const std::vector<IRoom*>& rooms) = 0;

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) = 0;

        virtual void focus() = 0;
    };
}

