#pragma once

#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class Window;
    }

    class Flipmaps
    {
    public:
        explicit Flipmaps(ui::Control& parent);

        /// Set whether to use alternate groups method of flipmaps.
        /// @param value Whether to use alternate groups or a single toggle.
        void set_alternate_groups(bool value);

        /// Set the current flip mode. This will not raise the on_flip event but will update the user interface appropriately.
        /// @param flip The new flip mode.
        void set_flip(bool flip);

        /// Set whether the flip control is enabled or disabled.
        /// @param enabled Whether the control is enabled.
        void set_flip_enabled(bool enabled);

        Event<bool> on_flip;
    private:
        ui::Checkbox* _flip;
        ui::Window*   _tr1_3_panel;
        ui::Window*   _tr4_5_panel;
    };
}
