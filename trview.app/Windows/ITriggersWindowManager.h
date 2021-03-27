#pragma once

#include "TriggersWindow.h"

namespace trview
{
    struct ITriggersWindowManager
    {
        using TriggersWindowSource = std::function<std::shared_ptr<ITriggersWindow> (const Window)>;

        virtual ~ITriggersWindowManager() = 0;

        /// Event raised when an item is selected in one of the trigger windows.
        Event<Item> on_item_selected;

        Event<Trigger*, bool> on_trigger_visibility;

        /// Event raised when a trigger is selected in one of the trigger windows.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed in one of the trigger windows.
        Event<const Trigger*> on_add_to_route;

        /// Render all of the triggers windows.
        /// @param vsync Whether to use vsync.
        virtual void render(bool vsync) = 0;

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<Item>& items) = 0;

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) = 0;

        /// Set whether a trigger is visible.
        /// @param trigger The trigger.
        /// @param visible Whether the trigger is visible.
        virtual void set_trigger_visible(Trigger* trigger, bool visible) = 0;

        /// Set the current room to filter trigger windows.
        /// @param room The current room.
        virtual void set_room(uint32_t room) = 0;

        /// Set the currently selected trigger.
        /// @param item The selected trigger.
        virtual void set_selected_trigger(const Trigger* const trigger) = 0;

        /// Create a new triggers window.
        virtual std::weak_ptr<ITriggersWindow> create_window() = 0;
    };
}
