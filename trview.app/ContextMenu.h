#pragma once

#include <trview.ui/Window.h>

namespace trview
{
    class ContextMenu final
    {
    public:
        /// Create a new ContentMenu window.
        /// @param parent The control to add the window to.
        explicit ContextMenu(ui::Control& parent);

        /// Set the position of the context menu.
        /// @param position The new position of the menu.
        void set_position(const Point& position);

        /// Set the context menu to visible.
        /// @param value The new visibility status.
        void set_visible(bool value);
    private:
        ui::Window* _menu;
    };
}
