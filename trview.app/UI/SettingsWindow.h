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
        class Slider;
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

        /// Event raised when the movement speed has been changed. The new movement speed value is passed
        /// as a parameter when the event is raised.
        /// @remarks This event is not raised by the set_movement_speed function.
        Event<float> on_movement_speed_changed;

        /// Event raised when the camera sensitivity has been changed by the user. The new sensitivity
        /// value is passed as a parameter when the event is raised.
        /// @remarks This event is not raised by the set_sensitivity function.
        Event<float> on_sensitivity_changed;

        /// Event raised when the 'triggers window at startup' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_triggers_startup;

        /// Event raised when the 'rooms window at startup' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_rooms_startup;

        /// Event raised when the 'Switch to orbit on selection' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_auto_orbit;

        /// Event raised when the 'Invert vertical pan' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_invert_vertical_pan;

        /// Event raised when the 'camera acceleration' setting has been changed. The new setting is passed as the parameter.
        Event<bool> on_camera_acceleration;

        /// Event raised when the 'camera acceleration rate' setting has been changed. The new setting is passed as the parameter.
        Event<float> on_camera_acceleration_rate;

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

        /// Set the new value of the 'rooms window at startup' setting. This will not raise the on_rooms_startup event.
        /// @param value The new 'rooms window at startup' setting.
        void set_rooms_startup(bool value);

        /// Set the new value of the 'Switch to orbit on selection' setting. This will not raise the on_auto_orbit event.
        /// @param value The new 'Switch to orbit on selection' setting.
        void set_auto_orbit(bool value);

        /// Set the new value of the 'camera acceleration' setting. This will not raise the on_camera_acceleration event.
        /// @param value The new 'camera acceleration' setting.
        void set_camera_acceleration(bool value);

        /// Set the new value of the 'camera acceleration rate' setting. This will not raise the on_camera_acceleration_rate event.
        /// @param value The new 'camera acceleration rate' setting.
        void set_camera_acceleration_rate(float value);

        /// Set the movement speed slider to specified value.
        /// @param value The movement speed between 0 and 1.
        /// @remarks This will not raise the on_movement_speed_changed event.
        void set_movement_speed(float value);

        /// Set the sensitivity slider to the specified value.
        /// @param value The sensitivity value between 0 and 1.
        /// @remarks This will not raise the on_sensitivity_changed event.
        void set_sensitivity(float value);

        /// Set the value of invert vertical pan.
        /// @param value The new setting value.
        /// @remarks This will not raise the on_invert_vertical_pan event.
        void set_invert_vertical_pan(bool value);

        /// Toggle the visibility of the settings window.
        void toggle_visibility();
    private:
        ui::Checkbox* _vsync{ nullptr };
        ui::Checkbox* _go_to_lara{ nullptr };
        ui::Checkbox* _invert_map_controls{ nullptr };
        ui::Checkbox* _items_startup{ nullptr };
        ui::Checkbox* _triggers_startup{ nullptr };
        ui::Checkbox* _rooms_startup{ nullptr };
        ui::Checkbox* _auto_orbit{ nullptr };
        ui::Slider* _sensitivity{ nullptr };
        ui::Slider* _movement_speed{ nullptr };
        ui::Control* _window{ nullptr };
        ui::Checkbox* _invert_vertical_pan{ nullptr };
        ui::Checkbox* _acceleration{ nullptr };
        ui::Slider* _acceleration_rate{ nullptr };
        TokenStore _token_store;
    };
}
