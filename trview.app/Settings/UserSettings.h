#pragma once

#include <list>
#include <string>

namespace trview
{
    struct UserSettings
    {
        void add_recent_file(const std::string& file);

        std::list<std::string>  recent_files;
        float                   camera_sensitivity{ 0 };
        float                   camera_movement_speed{ 0.5f };
        bool                    vsync{ true };
        bool                    go_to_lara{ true };
        bool                    invert_map_controls{ false };
        bool                    items_startup{ false };
        bool                    triggers_startup{ false };
        bool                    auto_orbit{ true };
        bool                    invert_vertical_pan{ true };
        uint32_t                background_colour = 0x003366;
        bool                    rooms_startup{ false };
        bool                    camera_acceleration{ true };
        float                   camera_acceleration_rate{ 0.5f };
    };
}

