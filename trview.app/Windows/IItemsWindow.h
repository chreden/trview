#pragma once

#include <vector>
#include <optional>
#include <trview.common/Event.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>

namespace trview
{
    struct IItemsWindow
    {
        virtual ~IItemsWindow() = 0;

        /// Event raised when an item is selected in the list.
        Event<Item> on_item_selected;

        /// Event raised when an item visibility is changed.
        Event<Item, bool> on_item_visibility;

        /// Event raised when a trigger is selected in the list.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed.
        Event<Item> on_add_to_route;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        /// Set the items to display in the window.
        /// @param items The items to show.
        virtual void set_items(const std::vector<Item>& items) = 0;

        /// Update the items - this doesn't reset the filters.
        virtual void update_items(const std::vector<Item>& items) = 0;

        /// Render the window.
        /// @param device The device to render with.
        /// @param vsync Whether to use vsync or not.
        virtual void render(const graphics::IDevice& device, bool vsync) = 0;

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) = 0;

        /// Clear the currently selected item from the details panel.
        virtual void clear_selected_item() = 0;

        /// Set the current room. This will be used when the track room setting is on.
        /// @param room The current room number.
        virtual void set_current_room(uint32_t room) = 0;

        /// Set the selected item.
        /// @param item The selected item.
        virtual void set_selected_item(const Item& item) = 0;

        /// Get the selected item.
        /// @returns The selected item, if present.
        virtual std::optional<Item> selected_item() const = 0;
    };
}

