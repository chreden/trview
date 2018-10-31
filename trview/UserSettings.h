#pragma once

#include <list>
#include <string>

namespace trview
{
    struct UserSettings
    {
        void add_recent_file(const std::wstring& file);

        std::list<std::wstring> recent_files;
        float                   camera_sensitivity{ 0 };
        float                   camera_movement_speed{ 0 };
        bool                    vsync{ true };
        bool                    go_to_lara{ true };
        bool                    invert_map_controls{ false };
        bool                    items_startup{ true };
        bool                    triggers_startup{ true };
        bool                    auto_orbit{ true };
    };

    // Load the user settings from the settings file.
    UserSettings load_user_settings();

    // Save the user settings to the settings file.
    void         save_user_settings(const UserSettings& settings);
}

