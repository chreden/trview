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
            inline static const std::string vsync = "VSync";
            inline static const std::string go_to_lara = "GoToLara";
            inline static const std::string invert_map_controls = "InvertMapControls";
            inline static const std::string items_startup = "ItemsStartup";
            inline static const std::string triggers_startup = "TriggersStartup";
            inline static const std::string rooms_startup = "RoomsStartup";
            inline static const std::string auto_orbit = "AutoOrbit";
            inline static const std::string invert_vertical_pan = "InvertVerticalPan";
            inline static const std::string camera_display_degrees = "CameraDisplayDegrees";
            inline static const std::string randomizer_tools = "RandomizerTools";
            inline static const std::string lau = "LAU";
            inline static const std::string max_recent_files = "MaxRecentFiles";
            inline static const std::string sensitivity = "Sensitivity";
            inline static const std::string movement_speed = "MovementSpeed";
            inline static const std::string acceleration = "Acceleration";
            inline static const std::string acceleration_rate = "AccelerationRate";
            inline static const std::string close = "Close";
        };

        /// <summary>
        /// Creates an instance of the SettingsWindow class. This will add UI elements to the control provided.
        /// </summary>
        /// <param name="parent">The control to which the instance will add elements.</param>
        /// <param name="source">The function to call to get the UI elements.</param>
        explicit SettingsWindow(ui::Control& parent, const std::shared_ptr<ui::ILoader>& source);
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
        virtual void set_randomizer_tools(bool value) override;
        virtual void set_lau(bool value) override;
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
        ui::Checkbox* _lau{ nullptr };
        ui::NumericUpDown* _max_recent_files{ nullptr };
        TokenStore _token_store;
    };
}
