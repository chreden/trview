/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

#include <trview.common/Event.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
    }
    struct ITextureStorage;

    /// UI window for program level settings.
    class SettingsWindow final
    {
    public:
        /// Creates an instance of the SettingsWindow class. This will add UI elements to the control provided.
        /// @param parent The control to which the instance will add elements.
        /// @param texture_storage The texture storage instance to use.
        explicit SettingsWindow(ui::Control& parent, const ITextureStorage& texture_storage);

        /// Event raised when the vsync settings has been changed. The new vsync setting is passed as the parameter.
        Event<bool> on_vsync;

        /// Event raised when the 'go to lara' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_go_to_lara;

        /// Event raised when the inverted map controls setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_invert_map_controls;

        /// Set the new value of the vsync setting. This will not raise the on_vsync event.
        /// @param value The new vsync setting.
        void set_vsync(bool value);

        /// Set the new value of the 'go to lara' setting. This will not raise the on_go_to_lara event.
        /// @param value The new 'go to lara' setting.
        void set_go_to_lara(bool value);

        /// Set the new value of the inverted map controls setting. This will not raise the on_invert_map_controls event.
        /// @param value The new inverted map controls setting.
        void set_invert_map_controls(bool value);

        /// Toggle the visibility of the settings window.
        void toggle_visibility();
    private:
        ui::Checkbox* _vsync;
        ui::Checkbox* _go_to_lara;
        ui::Checkbox* _invert_map_controls;
        ui::Control* _window;
        TokenStore _token_store;
    };
}
