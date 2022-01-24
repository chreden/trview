/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

#include "ISettingsWindow.h"
#include <trview.common/TokenStore.h>
#include <trview.ui/ILoader.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class NumericUpDown;
        class Slider;
    }

    /// UI window for program level settings.
    class SettingsWindow final : public ISettingsWindow
    {
    public:
        struct Names
        {
            static const std::string vsync;
            static const std::string go_to_lara;
            static const std::string invert_map_controls;
            static const std::string items_startup;
            static const std::string triggers_startup;
            static const std::string rooms_startup;
            static const std::string auto_orbit;
            static const std::string invert_vertical_pan;
            static const std::string camera_display_degrees;
            static const std::string randomizer_tools;
            static const std::string max_recent_files;
            static const std::string sensitivity;
            static const std::string movement_speed;
            static const std::string acceleration;
            static const std::string acceleration_rate;
            static const std::string close;
        };

        /// <summary>
        /// Creates an instance of the SettingsWindow class. This will add UI elements to the control provided.
        /// </summary>
        /// <param name="parent">The control to which the instance will add elements.</param>
        /// <param name="source">The function to call to get the UI elements.</param>
        explicit SettingsWindow(ui::Control& parent, const std::shared_ptr<ui::ILoader>& source);
        virtual ~SettingsWindow() = default;
        virtual void render() override;
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
        virtual void set_randomizer_tools(bool value) override;
        virtual void set_max_recent_files(uint32_t value) override;
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
        ui::Checkbox* _randomizer_tools{ nullptr };
        ui::NumericUpDown* _max_recent_files{ nullptr };
        TokenStore _token_store;
        bool _visible{ false };
    };
}
