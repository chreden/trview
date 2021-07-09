/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

#include "ISettingsWindow.h"
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class Slider;
    }

    /// UI window for program level settings.
    class SettingsWindow final : public ISettingsWindow
    {
    public:
        /// <summary>
        /// Creates an instance of the SettingsWindow class. This will add UI elements to the control provided.
        /// </summary>
        /// <param name="parent">The control to which the instance will add elements.</param>
        explicit SettingsWindow(ui::Control& parent);
        virtual ~SettingsWindow() = default;
        virtual void set_vsync(bool value) override;
        virtual void set_go_to_lara(bool value) override;
        virtual void set_invert_map_controls(bool value) override;
        virtual void set_items_startup(bool value) override;
        virtual void set_triggers_startup(bool value) override;
        virtual void set_rooms_startup(bool value) override;
        virtual void set_auto_orbit(bool value) override;
        virtual void set_camera_acceleration(bool value) override;
        virtual void set_camera_acceleration_rate(float value) override;
        virtual void set_movement_speed(float value) override;
        virtual void set_sensitivity(float value) override;
        virtual void set_invert_vertical_pan(bool value) override;
        virtual void set_camera_display_degrees(bool value) override;
        virtual void toggle_visibility() override;
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
        ui::Checkbox* _camera_display_degrees{ nullptr };
        TokenStore _token_store;
    };
}
