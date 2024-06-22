#pragma once

#include "TriggersWindow.h"

namespace trview
{
    struct ITriggersWindowManager
    {
        virtual ~ITriggersWindowManager() = 0;

        /// Event raised when an item is selected in one of the trigger windows.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when a trigger is selected in one of the trigger windows.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed in one of the trigger windows.
        Event<std::weak_ptr<ITrigger>> on_add_to_route;

        Event<uint32_t> on_camera_sink_selected;

        /// Render all of the triggers windows.
        virtual void render() = 0;

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) = 0;

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;

        /// <summary>
        /// Set the current room to filter trigger windows.
        /// </summary>
        /// <param name="room">The current room.</param>
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Set the currently selected trigger.
        /// @param item The selected trigger.
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;

        /// Create a new triggers window.
        virtual std::weak_ptr<ITriggersWindow> create_window() = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;
    };
}
