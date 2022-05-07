#pragma once

#include "../../UI/IViewerUI.h"

namespace trview
{
    namespace mocks
    {
        struct MockViewerUI : public IViewerUI
        {
            virtual ~MockViewerUI() = default;
            MOCK_METHOD(void, clear_minimap_highlight, (), (override));
            MOCK_METHOD(std::shared_ptr<ISector>, current_minimap_sector, (), (const, override));
            MOCK_METHOD(bool, is_input_active, (), (const, override));
            MOCK_METHOD(bool, is_cursor_over, (), (const, override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_alternate_group, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_alternate_groups, (const std::set<uint32_t>&), (override));
            MOCK_METHOD(void, set_camera_position, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, set_camera_rotation, (float, float), (override));
            MOCK_METHOD(void, set_camera_mode, (CameraMode), (override));
            MOCK_METHOD(void, set_camera_projection_mode, (ProjectionMode), (override));
            MOCK_METHOD(void, set_flip_enabled, (bool), (override));
            MOCK_METHOD(void, set_hide_enabled, (bool), (override));
            MOCK_METHOD(void, set_host_size, (const Size&), (override));
            MOCK_METHOD(void, set_level, (const std::string&, trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_max_rooms, (uint32_t), (override));
            MOCK_METHOD(void, set_measure_distance, (float), (override));
            MOCK_METHOD(void, set_measure_position, (const Point&), (override));
            MOCK_METHOD(void, set_minimap_highlight, (uint16_t, uint16_t), (override));
            MOCK_METHOD(void, set_pick, (const PickInfo&, const PickResult&), (override));
            MOCK_METHOD(void, set_remove_waypoint_enabled, (bool), (override));
            MOCK_METHOD(void, set_selected_item, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_room, (const std::shared_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_settings, (const UserSettings&), (override));
            MOCK_METHOD(void, set_show_context_menu, (bool), (override));
            MOCK_METHOD(void, set_show_measure, (bool), (override));
            MOCK_METHOD(void, set_show_minimap, (bool), (override));
            MOCK_METHOD(void, set_show_tooltip, (bool), (override));
            MOCK_METHOD(void, set_use_alternate_groups, (bool), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(bool, show_context_menu, (), (const, override));
            MOCK_METHOD(void, toggle_settings_visibility, (), (override));
            MOCK_METHOD(void, print_console, (const std::wstring&), (override));
            MOCK_METHOD(void, set_mid_waypoint_enabled, (bool), (override));
            MOCK_METHOD(void, set_toggle, (const std::string&, bool), (override));
            MOCK_METHOD(bool, toggle, (const std::string&), (const, override));
            MOCK_METHOD(void, set_scalar, (const std::string&, int32_t), (override));
        };
    }
}
