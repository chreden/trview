#include "UserSettings.h"

namespace trview
{
    namespace
    {
        void add_to_files_list(std::list<std::string>& files, const std::string& file, uint32_t max)
        {
            // If the file already exists in the recent files list, remove it from where it is
            // and move it to the to avoid duplicates and to make it ordered by most recent.
            auto existing = std::find(files.begin(), files.end(), file);
            if (existing != files.end())
            {
                files.erase(existing);
            }

            files.push_front(file);
            if (files.size() > max)
            {
                files.pop_back();
            }
        }
    }

    void UserSettings::add_recent_file(const std::string& file)
    {
        add_to_files_list(recent_files, file, max_recent_files);
    }

    void UserSettings::add_recent_diff_file(const std::string& file)
    {
        add_to_files_list(recent_diff_files, file, max_recent_files);
    }

    bool UserSettings::operator==(const UserSettings& other) const
    {
        return camera_sensitivity == other.camera_sensitivity &&
            camera_movement_speed == other.camera_movement_speed &&
            vsync == other.vsync &&
            go_to_lara == other.go_to_lara &&
            invert_map_controls == other.invert_map_controls &&
            items_startup == other.items_startup &&
            triggers_startup == other.triggers_startup &&
            auto_orbit == other.auto_orbit &&
            invert_vertical_pan == other.invert_vertical_pan &&
            background_colour == other.background_colour &&
            rooms_startup == other.rooms_startup &&
            camera_acceleration == other.camera_acceleration &&
            camera_display_degrees == other.camera_display_degrees &&
            randomizer_tools == other.randomizer_tools &&
            recent_files == other.recent_files &&
            max_recent_files == other.max_recent_files &&
            route_colour == other.route_colour &&
            waypoint_colour == other.waypoint_colour &&
            route_startup == other.route_startup &&
            fov == other.fov &&
            camera_sink_startup == other.camera_sink_startup &&
            statics_startup == other.statics_startup &&
            plugin_directories == other.plugin_directories &&
            camera_position_window == other.camera_position_window &&
            recent_diff_files == other.recent_diff_files &&
            items_window_columns == other.items_window_columns &&
            rooms_window_columns == other.rooms_window_columns &&
            statics_window_columns == other.statics_window_columns &&
            sounds_window_columns == other.sounds_window_columns &&
            lights_window_columns == other.lights_window_columns;
    }
}
