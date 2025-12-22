#pragma once

#include <trview.common/Event.h>
#include "IItemsWindow.h"

namespace trview
{
    struct IItemsWindowManager
    {
        virtual ~IItemsWindowManager() = 0;

        /// Event raised when the 'add to route' button is pressed in one of the item windows.
        Event<std::weak_ptr<IItem>> on_add_to_route;

        /// Render all of the item windows.
        virtual void render() = 0;

        /// <summary>
        /// Set the current room to filter item windows.
        /// </summary>
        /// <param name="room">The current room.</param>
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Create a new items window.
        virtual std::weak_ptr<IItemsWindow> create_window() = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;

        virtual std::vector<std::weak_ptr<IItemsWindow>> windows() const = 0;
    };
}
