#pragma once

#include "../../Windows/IViewer.h"

namespace trview
{
    namespace mocks
    {
        struct MockViewer : public IViewer
        {
            MockViewer();
            virtual ~MockViewer();
            MOCK_METHOD(CameraMode, camera_mode, (), (const, override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, open, (const std::weak_ptr<ILevel>&, ILevel::OpenMode), (override));
            MOCK_METHOD(void, set_settings, (const UserSettings&), (override));
            MOCK_METHOD(void, select_item, (const std::weak_ptr<IItem>&), (override));
            MOCK_METHOD(void, select_room, (uint32_t), (override));
            MOCK_METHOD(void, select_light, (const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, select_sector, (const std::weak_ptr<ISector>&), (override));
            MOCK_METHOD(void, select_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, select_waypoint, (const std::weak_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, select_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, select_static_mesh, (const std::weak_ptr<IStaticMesh>&), (override));
            MOCK_METHOD(void, set_camera_mode, (CameraMode), (override));
            MOCK_METHOD(void, set_route, (const std::shared_ptr<IRoute>&), (override));
            MOCK_METHOD(void, set_show_compass, (bool), (override));
            MOCK_METHOD(void, set_show_minimap, (bool), (override));
            MOCK_METHOD(void, set_show_route, (bool), (override));
            MOCK_METHOD(void, set_show_selection, (bool), (override));
            MOCK_METHOD(void, set_show_tools, (bool), (override));
            MOCK_METHOD(void, set_show_tooltip, (bool), (override));
            MOCK_METHOD(void, set_show_ui, (bool), (override));
            MOCK_METHOD(bool, ui_input_active, (), (const, override));
            MOCK_METHOD(void, present, (bool), (override));
            MOCK_METHOD(void, render_ui, (), (override));
            MOCK_METHOD(void, set_target, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, target, (), (const, override));
            MOCK_METHOD(void, set_scene_changed, (), (override));
        };
    }
}
