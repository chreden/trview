#pragma once

#include <trview.app/Windows/IViewer.h>

namespace trview
{
    namespace mocks
    {
        class MockViewer : public IViewer
        {
        public:
            MOCK_METHOD(CameraMode, camera_mode, (), (const, override));
            MOCK_METHOD(void, render, ());
            MOCK_METHOD(void, open, (ILevel*));
            MOCK_METHOD(void, set_settings, (const UserSettings&));
            MOCK_METHOD(void, select_item, (const Item&));
            MOCK_METHOD(void, select_room, (uint32_t));
            MOCK_METHOD(void, select_trigger, (const Trigger* const));
            MOCK_METHOD(void, select_waypoint, (const Waypoint&));
            MOCK_METHOD(void, set_camera_mode, (CameraMode), (override));
            MOCK_METHOD(void, set_route, (const std::shared_ptr<IRoute>&));
            MOCK_METHOD(void, set_show_compass, (bool));
            MOCK_METHOD(void, set_show_minimap, (bool));
            MOCK_METHOD(void, set_show_route, (bool));
            MOCK_METHOD(void, set_show_selection, (bool));
            MOCK_METHOD(void, set_show_tools, (bool));
            MOCK_METHOD(void, set_show_tooltip, (bool));
            MOCK_METHOD(void, set_show_ui, (bool));
            MOCK_METHOD(bool, ui_input_active, (), (const));
        };
    }
}
