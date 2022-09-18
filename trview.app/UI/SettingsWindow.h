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
            static inline const std::string vsync = "Vsync";
            static inline const std::string go_to_lara = "Select Lara when level is opened";
            static inline const std::string invert_map_controls = "Invert map controls";
            static inline const std::string items_startup = "Open Items Window at startup";
            static inline const std::string triggers_startup = "Open Triggers Window at startup";
            static inline const std::string rooms_startup = "Open Rooms Window at startup";
            static inline const std::string auto_orbit = "Switch to orbit on selection";
            static inline const std::string invert_vertical_pan = "Invert vertical panning";
            static inline const std::string camera_display_degrees = "Use degrees for camera angle display";
            static inline const std::string randomizer_tools = "Enable Randomizer Tools";
            static inline const std::string max_recent_files = "Recent Files";
            static inline const std::string sensitivity = "Sensitivity";
            static inline const std::string movement_speed = "Movement Speed";
            static inline const std::string acceleration = "Acceleration";
            static inline const std::string acceleration_rate = "Acceleration Rate";
            static inline const std::string background_colour = "Background Colour";
            static inline const std::string default_route_colour = "Default Route Colour";
            static inline const std::string default_waypoint_colour = "Default Waypoint Colour";
            static inline const std::string route_startup = "Open Route Window at startup";
            static inline const std::string fov = "Camera FOV";
            static inline const std::string camera_sink_startup = "Open Camera/Sink Window at startup";
            static inline const std::string reset_fov = "Reset##Fov";
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
        virtual void set_map_colours(const MapColours& colours) override;
        virtual void toggle_visibility() override;
        virtual void set_default_route_colour(const Colour& colour) override;
        virtual void set_default_waypoint_colour(const Colour& colour) override;
        virtual void set_route_startup(bool value) override;
        virtual void set_fov(float value) override;
        virtual void set_camera_sink_startup(bool value) override;
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
        Colour _default_route_colour{ Colour::Green };
        Colour _default_waypoint_colour{ Colour::White };
        MapColours _colours;
        bool _route_startup{ false };
        float _fov{ 45 };
        bool _camera_sink_startup{ false };
    };
}
