#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>
#include <trview.app/Elements/Room.h>

namespace trview
{
    struct IRoomsWindow
    {
        virtual ~IRoomsWindow() = 0;

        /// Event raised when the user has selected a room in the room window.
        Event<uint32_t> on_room_selected;

        /// Event raised when the user has selected an item in the room window.
        Event<Item> on_item_selected;

        /// Event raised when the user has selected a trigger in the room window.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        /// Render the window.
        /// @param device The device to render with.
        /// @param vsync Whether to use vsync or not.
        virtual void render(const graphics::IDevice& device, bool vsync) = 0;

        /// Set the current room that the viewer is focusing on.
        /// @param room The current room.
        virtual void set_current_room(uint32_t room) = 0;

        /// Set the items in the level.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<Item>& items) = 0;

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        virtual void set_rooms(const std::vector<Room*>& rooms) = 0;

        /// Set the item currently selected in the viewer.
        /// @param item The item currently selected.
        virtual void set_selected_item(const Item& item) = 0;

        /// Set the trigger currently selected in the viewer.
        /// @param trigger The trigger currently selected.
        virtual void set_selected_trigger(const Trigger* const trigger) = 0;

        /// Set the triggers in the level.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) = 0;
    };
}