#pragma once

#include <vector>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Filters/Filters.h"

namespace trview
{
    struct IItemsWindow
    {
        using Source = std::function<std::shared_ptr<IItemsWindow>()>;

        virtual ~IItemsWindow() = 0;
        /// Event raised when the 'add to route' button is pressed.
        Event<std::weak_ptr<IItem>> on_add_to_route;
        /// Event raised when the window is closed.
        Event<> on_window_closed;
        virtual void set_filters(std::vector<Filters<IItem>::Filter> filters) = 0;
        /// Render the window.
        virtual void render() = 0;
        /// Clear the currently selected item from the details panel.
        virtual void clear_selected_item() = 0;
        /// Get the selected item.
        /// @returns The selected item, if present.
        virtual std::weak_ptr<IItem> selected_item() const = 0;
        /// <summary>
        /// Update the window.
        /// </summary>
        /// <param name="delta">Elapsed time since previous update.</param>
        virtual void update(float delta) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual std::string name() const = 0;
    };
}

