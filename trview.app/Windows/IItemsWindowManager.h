#pragma once

#include <trview.common/Event.h>
#include "ItemsWindow.h"

namespace trview
{
    struct IItemsWindowManager
    {
        virtual ~IItemsWindowManager() = 0;

        /// Event raised when an item is selected in one of the item windows.
        Event<Item> on_item_selected;

        Event<Item, bool> on_item_visibility;

        /// Event raised when a trigger is selected in one of the item windows.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed in one of the item windows.
        Event<Item> on_add_to_route;

        /// Render all of the item windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        virtual void render(bool vsync) = 0;

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<Item>& items) = 0;

        /// Set whether an item is visible.
        /// @param item The item.
        /// @param visible Whether the item is visible.
        virtual void set_item_visible(const Item& item, bool visible) = 0;

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) = 0;

        /// Set the current room to filter item windows.
        /// @param room The current room.
        virtual void set_room(uint32_t room) = 0;

        /// Set the currently selected item.
        /// @param item The selected item.
        virtual void set_selected_item(const Item& item) = 0;

        /// Create a new items window.
        virtual std::weak_ptr<IItemsWindow> create_window() = 0;
    };
}
