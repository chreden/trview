/// @file Neighbours.h
/// @brief UI element that allows the user to toggle neighbour mode and to control the level of depth.
/// 
/// Adds a small UI element that has a checkbox for neighbour mode and also allows the user to control
/// the level of depth that will be rendered.

#pragma once

#include <trview.common/Event.h>
#include <cstdint>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class NumericUpDown;
    }

    struct ITextureStorage;

    /// The neighbours control has settings for whether neighbour mode is enabled and to allow
    /// the user to select the depth of neighbours to display.
    class Neighbours
    {
    public:
        /// Creates an instance of the Neighbours class. This will add UI elements to the control specified.
        /// @param control The control to which to add UI elements.
        /// @param texture_storage The texture storage instance to use.
        Neighbours(ui::Control& control, const ITextureStorage& texture_storage);

        /// Event raised when the user has enabled or disabled neighbour mode. The boolean passed to when the event is
        /// raised indicates whether neighbours mode is enabled.
        /// @remarks This event is not raised when the set_enabled function is called.
        Event<bool> on_enabled_changed;

        /// Event raised when the user has changed the depth of neighbour to display. The newly selected depth is passed
        /// when the event is raised.
        Event<int32_t> on_depth_changed;

        /// Set whether neighbours are enabled. This will not raise the on_enabled_changed event.
        /// @param value Whether neighbours are enabled.
        void set_enabled(bool value);

        /// Set the value of the depth control. This will not raise the on_depth_changed event.
        /// @param value The neighbour depth to use.
        void set_depth(int32_t value);
    private:
        ui::Checkbox*      _enabled;
        ui::NumericUpDown* _depth;
    };
}
