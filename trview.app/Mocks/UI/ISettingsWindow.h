#pragma once

#include "../../UI/ISettingsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockSettingsWindow : public ISettingsWindow
        {
            MockSettingsWindow();
            virtual ~MockSettingsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_vsync, (bool), (override));
            MOCK_METHOD(void, set_go_to_lara, (bool), (override));
            MOCK_METHOD(void, set_invert_map_controls, (bool), (override));
            MOCK_METHOD(void, set_items_startup, (bool), (override));
            MOCK_METHOD(void, set_triggers_startup, (bool), (override));
            MOCK_METHOD(void, set_rooms_startup, (bool), (override));
            MOCK_METHOD(void, set_auto_orbit, (bool), (override));
            MOCK_METHOD(void, set_camera_acceleration, (bool), (override));
            MOCK_METHOD(void, set_camera_acceleration_rate, (float), (override));
            MOCK_METHOD(void, set_default_route_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_default_waypoint_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_movement_speed, (float), (override));
            MOCK_METHOD(void, set_sensitivity, (float), (override));
            MOCK_METHOD(void, set_invert_vertical_pan, (bool), (override));
            MOCK_METHOD(void, set_camera_display_degrees, (bool), (override));
            MOCK_METHOD(void, set_randomizer_tools, (bool), (override));
            MOCK_METHOD(void, set_max_recent_files, (uint32_t), (override));
            MOCK_METHOD(void, set_background_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_map_colours, (const MapColours&), (override));
            MOCK_METHOD(void, toggle_visibility, (), (override));
            MOCK_METHOD(void, set_route_startup, (bool), (override));
            MOCK_METHOD(void, set_fov, (float), (override));
            MOCK_METHOD(void, set_camera_sink_startup, (bool), (override));
            MOCK_METHOD(void, set_statics_startup, (bool), (override));
        };
    }
}
