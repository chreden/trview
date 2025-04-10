#pragma once

#include <list>
#include <string>
#include "RandomizerSettings.h"
#include "../UI/MapColours.h"
#include "FontSetting.h"
#include "PluginSetting.h"

namespace trview
{
    struct UserSettings
    {
        void add_recent_file(const std::string& file);
        void add_recent_diff_file(const std::string& file);

        struct RecentRoute
        {
            std::string route_path;
            bool is_rando{ false };
        };

        struct WindowPlacement
        {
            int32_t min_x;
            int32_t min_y;
            int32_t max_x;
            int32_t max_y;
            int32_t normal_left;
            int32_t normal_top;
            int32_t normal_right;
            int32_t normal_bottom;
        };

        std::list<std::string>  recent_files;
        float camera_sensitivity{ 0 };
        float camera_movement_speed{ 0.5f };
        bool vsync{ true };
        bool go_to_lara{ true };
        bool invert_map_controls{ false };
        bool items_startup{ false };
        bool triggers_startup{ false };
        bool auto_orbit{ true };
        bool invert_vertical_pan{ true };
        uint32_t background_colour = 0x003366;
        bool rooms_startup{ false };
        bool camera_acceleration{ true };
        float camera_acceleration_rate{ 0.5f };
        bool camera_display_degrees{ true };
        bool randomizer_tools{ false };
        RandomizerSettings randomizer;
        uint32_t max_recent_files{ 10u };
        MapColours map_colours;
        Colour route_colour{ Colour::Green };
        Colour waypoint_colour{ Colour::White };
        bool route_startup{ false };
        std::optional<WindowPlacement> window_placement;
        bool camera_sink_startup{ false };
        std::unordered_map<std::string, RecentRoute> recent_routes;
        float fov{ 45 };
        std::vector<std::string> plugin_directories;
        std::unordered_map<std::string, bool> toggles;
        std::unordered_map<std::string, FontSetting> fonts
        {
            { "Default", {.name = "Arial", .filename = "arial.ttf", .size = 12 } },
            { "Console", {.name = "Consolas", .filename = "consola.ttf", .size = 12 } }
        };
        bool statics_startup{ false };
        bool camera_position_window{ true };
        std::list<std::string> recent_diff_files;
        std::unordered_map<std::string, PluginSetting> plugins;

        bool operator==(const UserSettings& other) const;
    };
}

