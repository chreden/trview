/// @file GoToRoom.h
/// @brief UI element that allows the user to control the flipmaps in a level.

#pragma once

#include <cstdint>
#include <set>
#include <trview.common/Event.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class Button;
        class Control;
        class Checkbox;
        class Window;
        class StackPanel;
    }

    /// UI element that allows the user to control the flipmaps in a level.
    class Flipmaps final
    {
    public:
        /// Create a new Flipmaps window.
        /// @param parent The control to add the window to.
        explicit Flipmaps(ui::Control& parent);

        /// Set whether to use alternate groups method of flipmaps.
        /// @param value Whether to use alternate groups or a single toggle.
        void set_use_alternate_groups(bool value);

        /// Set whether an alternate group is enabled. This will not raise the on_alternate_group event.
        /// @param value The group to change.
        /// @param enabled Whether the group is enabled.
        void set_alternate_group(uint16_t value, bool enabled);

        /// Set the alternate groups that are in the level.
        /// @param groups The groups in the level.
        void set_alternate_groups(const std::set<uint16_t>& groups);

        /// Set the current flip mode. This will not raise the on_flip event but will update the user interface appropriately.
        /// @param flip The new flip mode.
        void set_flip(bool flip);

        /// Set whether the flip control is enabled or disabled.
        /// @param enabled Whether the control is enabled.
        void set_flip_enabled(bool enabled);

        /// Event raised when a TR1-3 flip in toggled. The flipmap state is passed as a parameter.
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
        std::unordered_map<uint16_t, ui::Button*> _alternate_group_buttons;
    };
}
