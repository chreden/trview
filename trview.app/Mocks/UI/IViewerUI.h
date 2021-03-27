#pragma once

#include "../../UI/IViewerUI.h"

namespace trview
{
    namespace mocks
    {
        class MockViewerUI final : public IViewerUI
        {
        public:
            virtual ~MockViewerUI() = default;
            MOCK_METHOD(void, clear_minimap_highlight, ());
            MOCK_METHOD(std::shared_ptr<Sector>, current_minimap_sector, (), (const));
            MOCK_METHOD(bool, is_input_active, (), (const));
            MOCK_METHOD(bool, is_cursor_over, (), (const));
            MOCK_METHOD(void, render, ());
            MOCK_METHOD(void, set_alternate_group, (uint32_t, bool));
            MOCK_METHOD(void, set_alternate_groups, (const std::set<uint32_t>&));
            MOCK_METHOD(void, set_camera_position, (const DirectX::SimpleMath::Vector3&));
            MOCK_METHOD(void, set_camera_mode, (CameraMode));
            MOCK_METHOD(void, set_camera_projection_mode, (ProjectionMode));
            MOCK_METHOD(void, set_depth_enabled, (bool));
            MOCK_METHOD(void, set_depth_level, (int32_t));
            MOCK_METHOD(void, set_flip, (bool));
            MOCK_METHOD(void, set_flip_enabled, (bool));
            MOCK_METHOD(void, set_hide_enabled, (bool));
            MOCK_METHOD(void, set_highlight, (bool));
            MOCK_METHOD(void, set_host_size, (const Size&));
            MOCK_METHOD(void, set_level, (const std::string&, trlevel::LevelVersion));
            MOCK_METHOD(void, set_max_rooms, (uint32_t));
            MOCK_METHOD(void, set_measure_distance, (float));
            MOCK_METHOD(void, set_measure_position, (const Point&));
            MOCK_METHOD(void, set_minimap_highlight, (uint16_t, uint16_t));
            MOCK_METHOD(void, set_pick, (const PickInfo&, const PickResult&));
            MOCK_METHOD(void, set_remove_waypoint_enabled, (bool));
            MOCK_METHOD(void, set_selected_room, (Room*));
            MOCK_METHOD(void, set_settings, (const UserSettings&));
            MOCK_METHOD(void, set_show_context_menu, (bool));
            MOCK_METHOD(void, set_show_hidden_geometry, (bool));
            MOCK_METHOD(void, set_show_measure, (bool));
            MOCK_METHOD(void, set_show_minimap, (bool));
            MOCK_METHOD(void, set_show_tooltip, (bool));
            MOCK_METHOD(void, set_show_triggers, (bool));
            MOCK_METHOD(void, set_show_water, (bool));
            MOCK_METHOD(void, set_show_wireframe, (bool));
            MOCK_METHOD(void, set_use_alternate_groups, (bool));
            MOCK_METHOD(void, set_visible, (bool));
            MOCK_METHOD(bool, show_hidden_geometry, (), (const));
            MOCK_METHOD(bool, show_triggers, (), (const));
            MOCK_METHOD(bool, show_water, (), (const));
            MOCK_METHOD(bool, show_wireframe, (), (const));
            MOCK_METHOD(bool, show_context_menu, (), (const));
            MOCK_METHOD(void, toggle_settings_visibility, ());
            MOCK_METHOD(void, print_console, (const std::wstring&));
            MOCK_METHOD(void, initialise_input, ());
        };
    }
}
