#pragma once

#include <vector>
#include <optional>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"

namespace trview
{
    struct IItemsWindow
    {
        using Source = std::function<std::shared_ptr<IItemsWindow>()>;

        virtual ~IItemsWindow() = 0;

        /// Event raised when an item is selected in the list.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when a trigger is selected in the list.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed.
        Event<std::weak_ptr<IItem>> on_add_to_route;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        /// Set the items to display in the window.
        /// @param items The items to show.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;

        /// Render the window.
        virtual void render() = 0;

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;

        /// Clear the currently selected item from the details panel.
        virtual void clear_selected_item() = 0;

        /// <summary>
        /// Set the current room. This will be used when the track room setting is on.
        /// </summary>
        /// <param name="room">The current room.</param>
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Set the selected item.
        /// @param item The selected item.
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) = 0;

        /// Get the selected item.
        /// @returns The selected item, if present.
        virtual std::weak_ptr<IItem> selected_item() const = 0;
        /// <summary>
        /// Update the window.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual void set_number(int32_t number) = 0;

        virtual void set_level_version(trlevel::LevelVersion version) = 0;

        virtual void set_model_checker(const std::function<bool(uint32_t)>& checker) = 0;
    };
}

