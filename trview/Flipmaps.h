#pragma once

#include <cstdint>
#include <set>
#include <trview.common/Event.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class Window;
        class StackPanel;
    }

    class Flipmaps
    {
    public:
        explicit Flipmaps(ui::Control& parent);

        /// Set whether to use alternate groups method of flipmaps.
        /// @param value Whether to use alternate groups or a single toggle.
        void set_use_alternate_groups(bool value);

        /// Set the alternate groups that are in the level.
        /// @param groups The groups in the level.
        void set_alternate_groups(const std::set<uint16_t>& groups);

        /// Set the current flip mode. This will not raise the on_flip event but will update the user interface appropriately.
        /// @param flip The new flip mode.
        void set_flip(bool flip);

        /// Set whether the flip control is enabled or disabled.
        /// @param enabled Whether the control is enabled.
        void set_flip_enabled(bool enabled);

        Event<bool> on_flip;

        /// Event raised when an alternate group is toggled. The group number and the new state are passed as parameters.
        Event<uint16_t, bool> on_alternate_group;
    private:
        TokenStore      _token_store;
        ui::Checkbox*   _flip;
        ui::Window*     _tr1_3_panel;
        ui::Window*     _tr4_5_panel;
        ui::StackPanel* _alternate_groups;
        std::unordered_map<uint16_t, bool> _alternate_group_values;
    };
}
