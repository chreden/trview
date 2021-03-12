#pragma once

#include <cstdint>
#include <SimpleMath.h>

#include <trview.common/Event.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>
#include <trview.app/Elements/ILevel.h>
#include <trview.app/Routing/Route.h>
#include <trview.app/Routing/Waypoint.h>
#include <trview.app/Settings/UserSettings.h>

namespace trview
{
    struct IViewer
    {
        virtual ~IViewer() = 0;

        /// Event raised when the user settings have changed.
        /// @remarks The settings is passed as a parameter to the listener functions.
        Event<UserSettings> on_settings;

        /// Event raised when the viwer wants to select an item.
        Event<Item> on_item_selected;

        /// Event raised when the viewer wants to change the visibility of an item.
        Event<Item, bool> on_item_visibility;

        /// Event raised when the viewer wants to select a room.
        Event<uint32_t> on_room_selected;

        /// Event raised when the viewer wants to select a trigger.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the viewer wants to change the visibility of a trigger.
        Event<Trigger*, bool> on_trigger_visibility;

        /// Event raised when the viewer wants to select a waypoint.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when the viewer wants to remove a waypoint.
        Event<uint32_t> on_waypoint_removed;

        /// Event raised when the viewer wants to add a waypoint.
        Event<DirectX::SimpleMath::Vector3, uint32_t, Waypoint::Type, uint32_t> on_waypoint_added;

        /// Render the viewer.
        virtual void render() = 0;

        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        virtual void open(ILevel* level) = 0;

        virtual void set_settings(const UserSettings& settings) = 0;

        /// Select the specified item.
        /// @param item The item to select.
        /// @remarks This will not raise the on_item_selected event.
        virtual void select_item(const Item& item) = 0;

        /// Select the specified room.
        /// @param room The room to select.
        /// @remarks This will not raise the on_room_selected event.
        virtual void select_room(uint32_t room) = 0;

        /// Select the specified trigger.
        /// @param trigger The trigger to select.
        /// @remarks This will not raise the on_trigger_selected event.
        virtual void select_trigger(const Trigger* const trigger) = 0;

        /// Select the specified waypoint
        /// @param index The waypoint to select.
        /// @remarks This will not raise the on_waypoint_selected event.
        virtual void select_waypoint(const Waypoint& waypoint) = 0;

        /// Set the current route.
        /// @param route The new route.
        virtual void set_route(IRoute* route) = 0;

        /// Set whether the compass is visible.
        virtual void set_show_compass(bool value) = 0;

        /// Set whether the minimap is visible.
        virtual void set_show_minimap(bool value) = 0;

        /// Set whether the route is visible.
        virtual void set_show_route(bool value) = 0;

        /// Set whether the selection is visible.
        virtual void set_show_selection(bool value) = 0;

        /// Set whether the tools are visible.
        virtual void set_show_tools(bool value) = 0;

        /// Set whether the tooltip is visible.
        virtual void set_show_tooltip(bool value) = 0;

        /// Set whether the ui is visible.
        virtual void set_show_ui(bool value) = 0;

        virtual bool ui_input_active() const = 0;
    };
}
