#include "UserSettings.h"

namespace trview
{
    void UserSettings::add_recent_file(const std::string& file)
    {
        // If the file already exists in the recent files list, remove it from where it is
        // and move it to the to avoid duplicates and to make it ordered by most recent.
        auto existing = std::find(recent_files.begin(), recent_files.end(), file);
        if (existing != recent_files.end())
        {
            recent_files.erase(existing);
        }

        recent_files.push_front(file);
        if (recent_files.size() > max_recent_files)
        {
            recent_files.pop_back();
        }
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
            plugins_directory == other.plugins_directory;
    }
}
