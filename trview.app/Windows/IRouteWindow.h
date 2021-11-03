#pragma once

#include <trview.common/Event.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/IRoom.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.app/Routing/IRoute.h>
#include <trview.app/Settings/RandomizerSettings.h>

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
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the route colour has been changed.
        Event<Colour> on_colour_changed;

        /// Event raised when a route file is opened.
        Event<std::string, bool> on_route_import;

        /// Event raised when a route is exported.
        Event<std::string, bool> on_route_export;

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

