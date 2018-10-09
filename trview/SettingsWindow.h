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
        explicit SettingsWindow(ui::Control& parent);

        /// Event raised when the vsync settings has been changed. The new vsync setting is passed as the parameter.
        Event<bool> on_vsync;

        /// Event raised when the 'go to lara' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_go_to_lara;

        /// Event raised when the inverted map controls setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_invert_map_controls;

        /// Event raised when the 'items window at startup' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_items_startup;

        /// Event raised when the 'triggers window at startup' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_triggers_startup;

        /// Set the new value of the vsync setting. This will not raise the on_vsync event.
        /// @param value The new vsync setting.
        void set_vsync(bool value);

        /// Set the new value of the 'go to lara' setting. This will not raise the on_go_to_lara event.
        /// @param value The new 'go to lara' setting.
        void set_go_to_lara(bool value);

        /// Set the new value of the inverted map controls setting. This will not raise the on_invert_map_controls event.
        /// @param value The new inverted map controls setting.
        void set_invert_map_controls(bool value);

        /// Set the new value of the 'items window at startup' setting. This will not raise the on_items_startup event.
        /// @param value The new 'items window at startup' setting.
        void set_items_startup(bool value);

        /// Set the new value of the 'triggers window at startup' setting. This will not raise the on_triggers_startup event.
        /// @param value The new 'triggers window at startup' setting.
        void set_triggers_startup(bool value);

        /// Toggle the visibility of the settings window.
        void toggle_visibility();
    private:
        ui::Checkbox* _vsync;
        ui::Checkbox* _go_to_lara;
        ui::Checkbox* _invert_map_controls;
        ui::Checkbox* _items_startup;
        ui::Checkbox* _triggers_startup;
        ui::Control* _window;
        TokenStore _token_store;
    };
}
