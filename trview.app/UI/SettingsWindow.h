/// @file SettingsWindow.h
/// @brief UI window for program level settings.

#pragma once

#include "ISettingsWindow.h"

namespace trview
{
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
        virtual void set_background_colour(const Colour& colour) override;
        virtual void toggle_visibility() override;
    private:
        bool _visible{ false };
        bool _vsync{ false };
        bool _go_to_lara{ false };
        bool _invert_map_controls{ false };
        bool _items_startup{ false };
        bool _triggers_startup{ false };
        bool _rooms_startup{ false };
        bool _auto_orbit{ false };
        bool _invert_vertical_panning{ false };
        bool _camera_display_degrees{ false };
        bool _randomizer_tools{ false };
        bool _acceleration{ false };
        float _sensitivity{ 1.0f };
        float _acceleration_rate{ 1.0f };
        float _movement_speed{ 1.0f };
        int _max_recent_files{ 10 };
        float _colour[3];
    };
}
