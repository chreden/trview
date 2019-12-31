#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include <set>

#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class NumericUpDown;
        class Button;
        class Window;
        class StackPanel;
    }

    struct ITextureStorage;

    class ViewOptions final
    {
    public:
        explicit ViewOptions(ui::Control& parent, const ITextureStorage& texture_storage);

        /// Event raised when an alternate group is toggled. The group number and the new state are passed as parameters.
        Event<uint32_t, bool> on_alternate_group;

        /// Event raised when the user has changed the depth of neighbour to display. The newly selected depth is passed
        /// when the event is raised.
        Event<int32_t> on_depth_changed;

        /// Event raised when the user has enabled or disabled neighbour mode. The boolean passed to when the event is
        /// raised indicates whether neighbours mode is enabled.
        /// @remarks This event is not raised when the set_enabled function is called.
        Event<bool> on_depth_enabled;

        /// Event raised when the user toggles the alternate mode. The boolean passed as a parameter when this
        /// event is raised indicates whether flip mode is enabled.
        /// @remarks This event is not raised by the set_flip function.
        Event<bool> on_flip;
        
        /// Event raised when the user toggles the highlight mode. The boolean passed as a parameter when this
        /// event is raised indicates whether highlight mode is enabled.
        /// @remarks This event is not raised by the set_highlight function.
        Event<bool> on_highlight;

        /// Event raised when the user toggles the hidden geometry visibility. The boolean passed as a parameter when
        /// this event is raised indicates whether hidden geomety is visible.
        /// @remarks This event is not raised by the set_show_hidden_geometry function.
        Event<bool> on_show_hidden_geometry;

        /// Event raised when the user toggles the trigger visibility. The boolean passed as a parameter when this
        /// event is raised indicates whether triggers are visible.
        /// @remarks This event is not raised by the set_show_triggers function.
        Event<bool> on_show_triggers;

        /// Event raised when the user toggles showing water. The boolean passed as a paramter when this event is raised
        /// indicates whether water colouring is visible.
        /// @remarks This event is not raised by the set_show_water function.
        Event<bool> on_show_water;

        /// Set whether an alternate group is enabled. This will not raise the on_alternate_group event.
        /// @param value The group to change.
        /// @param enabled Whether the group is enabled.
        void set_alternate_group(uint32_t value, bool enabled);

        /// Set the alternate groups that are in the level.
        /// @param groups The groups in the level.
        void set_alternate_groups(const std::set<uint32_t>& groups);

        /// Set the value of the depth control. This will not raise the on_depth_changed event.
        /// @param value The neighbour depth to use.
        void set_depth(int32_t value);
        
        /// Set whether neighbours are enabled. This will not raise the on_enabled_changed event.
        /// @param value Whether neighbours are enabled.
        void set_depth_enabled(bool value);

        /// Set the current flip mode. This will not raise the on_flip event but will update the user interface appropriately.
        /// @param flip The new flip mode.
        void set_flip(bool flip);

        /// Set whether the flip control is enabled or disabled.
        /// @param enabled Whether the control is enabled.
        void set_flip_enabled(bool enabled);

        /// Set the current highlight mode. This will not raise the on_highlight event but will update the user interface appropriately.
        /// @param highlight Whether the highlight mode is enabled or disabled.
        void set_highlight(bool highlight);

        /// Set whether hidden geometry is visible or not.
        /// @param show Whether the hidden geometry is visible.
        void set_show_hidden_geometry(bool show);

        /// Set whether triggers are visible or not.
        /// @param show Whether the triggers are visible.
        void set_show_triggers(bool show);

        /// Set whether water is visible or not.
        /// @param show Whether water is visible.
        void set_show_water(bool show);

        /// Set whether to use alternate groups method of flipmaps.
        /// @param value Whether to use alternate groups or a single toggle.
        void set_use_alternate_groups(bool value);

        /// Get the current value of the show hidden geometry checkbox.
        /// @returns The current value of the checkbox.
        bool show_hidden_geometry() const;
        
        /// Get the current value of the show triggers checkbox.
        /// @returns The current value of the checkbox.
        bool show_triggers() const;

        /// Get the current value of the show water checkbox.
        /// @returns The current value of the checkbox.
        bool show_water() const;
    private:
        TokenStore _token_store;
        ui::Checkbox* _highlight;
        ui::Checkbox* _flip;
        ui::Checkbox* _triggers;
        ui::Checkbox* _hidden_geometry;
        ui::Checkbox* _water;
        ui::Checkbox* _enabled;
        ui::NumericUpDown* _depth;
        ui::Window* _tr1_3_panel;
        ui::Window* _tr4_5_panel;
        ui::StackPanel* _alternate_groups;
        std::unordered_map<uint32_t, bool> _alternate_group_values;
        std::unordered_map<uint32_t, ui::Button*> _alternate_group_buttons;
    };
}
