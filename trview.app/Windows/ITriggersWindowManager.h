#pragma once

#include "ITriggersWindow.h"

namespace trview
{
    struct ITriggersWindowManager
    {
        virtual ~ITriggersWindowManager() = 0;

        /// Event raised when the 'add to route' button is pressed in one of the trigger windows.
        Event<std::weak_ptr<ITrigger>> on_add_to_route;

        /// Render all of the triggers windows.
        virtual void render() = 0;
        /// Create a new triggers window.
        virtual std::weak_ptr<ITriggersWindow> create_window() = 0;
        /// <summary>
        /// Update the windows.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;
    };
}
