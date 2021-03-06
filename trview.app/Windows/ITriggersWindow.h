#pragma once

#include <vector>
#include <optional>
#include <trview.common/Event.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/ITrigger.h>
#include <trview.ui/Control.h>

namespace trview
{
    struct ITriggersWindow
    {
        using Source = std::function<std::shared_ptr<ITriggersWindow>(const Window)>;

        virtual ~ITriggersWindow() = 0;

        /// Event raised when a trigger is selected in the list.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the visibility of a trigger is changed.
        Event<std::weak_ptr<ITrigger>, bool> on_trigger_visibility;

        /// Event raised when an item is selected in the list.
        Event<Item> on_item_selected;

        /// Event raised when the 'add to route' button is pressed.
        Event<std::weak_ptr<ITrigger>> on_add_to_route;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        /// Clear the currently selected trigger from the details panel.
        virtual void clear_selected_trigger() = 0;

        /// Render the window.
        /// @param vsync Whether to use vsync or not.
        virtual void render(bool vsync) = 0;

        virtual std::weak_ptr<ITrigger> selected_trigger() const = 0;

        /// Set the current room. This will be used when the track room setting is on.
        /// @param room The current room number.
        virtual void set_current_room(uint32_t room) = 0;

        virtual void set_items(const std::vector<Item>& items) = 0;

        /// Set the selected trigger.
        /// @param item The selected trigger.
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        /// @param reset_filters Whether to reset the trigger filters.
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;

        /// Update the trigers - this doesn't reset the filters.
        virtual void update_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;
    };
}

