#pragma once

#include <vector>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "../Elements/CameraSink/ICameraSink.h"

namespace trview
{
    struct ITriggersWindow
    {
        using Source = std::function<std::shared_ptr<ITriggersWindow>()>;

        virtual ~ITriggersWindow() = 0;

        /// Event raised when the 'add to route' button is pressed.
        Event<std::weak_ptr<ITrigger>> on_add_to_route;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        /// Clear the currently selected trigger from the details panel.
        virtual void clear_selected_trigger() = 0;

        /// Render the window.
        virtual void render() = 0;

        virtual std::weak_ptr<ITrigger> selected_trigger() const = 0;

        /// <summary>
        /// Set the current room. This will be used when the track room setting is on.
        /// </summary>
        /// <param name="room">The current room number</param>
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_number(int32_t number) = 0;
        /// <summary>
        /// Update the window.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;
    };
}

