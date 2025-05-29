#pragma once

#include <trview.common/Event.h>
#include "IItemsWindow.h"

namespace trview
{
    struct IItemsWindowManager
    {
        virtual ~IItemsWindowManager() = 0;

        /// Event raised when an item is selected in one of the item windows.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when a trigger is selected in one of the item windows.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed in one of the item windows.
        Event<std::weak_ptr<IItem>> on_add_to_route;

        Event<UserSettings> on_settings;

        /// Render all of the item windows.
        virtual void render() = 0;

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;

        virtual void set_level_version(trlevel::LevelVersion version) = 0;

        virtual void set_model_checker(const std::function<bool (uint32_t)>& checker) = 0;

        virtual void set_ng_plus(bool value) = 0;

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;

        /// <summary>
        /// Set the current room to filter item windows.
        /// </summary>
        /// <param name="room">The current room.</param>
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Set the currently selected item.
        /// @param item The selected item.
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) = 0;

        /// Create a new items window.
        virtual std::weak_ptr<IItemsWindow> create_window() = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual std::vector<std::weak_ptr<IItemsWindow>> windows() const = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
    };
}
