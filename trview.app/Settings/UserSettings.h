#pragma once

#include <list>
#include <string>
#include "RandomizerSettings.h"
#include "../UI/MapColours.h"
#include "FontSetting.h"
#include "PluginSetting.h"
#include "../Menus/LevelSortingMode.h"

namespace trview
{
    struct UserSettings
    {
        struct Fonts
        {
            static const inline FontSetting Default{ .name = "Arial", .filename = "arial.ttf", .size = 12 };
            static const inline FontSetting Console{ .name = "Consolas", .filename = "consola.ttf", .size = 12 };
            static const inline FontSetting Minimap{ .name = "Consolas", .filename = "consola.ttf", .size = 9 };
        };

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
        int32_t max_recent_files{ 10u };
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
            { "Default", Fonts::Default },
            { "Console", Fonts::Console },
            { "Minimap", Fonts::Minimap }
        };
        bool statics_startup{ false };
        bool camera_position_window{ true };
        std::list<std::string> recent_diff_files;
        std::unordered_map<std::string, PluginSetting> plugins;
        LevelSortingMode level_sorting_mode{ LevelSortingMode::Full };
        std::vector<std::string> items_window_columns{ "#", "Room", "Type ID", "Type", "Hide" };
        std::vector<std::string> rooms_window_columns{ "#", "Items", "Triggers", "Statics", "Hide" };
        std::vector<std::string> statics_window_columns{ "#", "Room", "ID", "Type", "Hide" };
        std::vector<std::string> sounds_window_columns{ "#", "ID", "Sample", "Hide" };
        std::vector<std::string> lights_window_columns{ "#", "Type", "Room", "Hide" };
        std::vector<std::string> camera_sink_window_columns{ "#", "Type", "Room", "Hide" };
        std::vector<std::string> triggers_window_columns{ "#", "Type", "Room", "Hide" };
        std::vector<std::string> flyby_columns{ "#", "Hide" };
        std::vector<std::string> flyby_node_columns{ "#", "Room" };
        bool linear_filtering{ false };
        std::string version;

        bool operator==(const UserSettings& other) const;
    };
}

