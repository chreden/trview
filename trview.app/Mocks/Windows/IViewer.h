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
            MOCK_METHOD(std::weak_ptr<ICamera>, camera, (), (const, override));
            MOCK_METHOD(ICamera::Mode, camera_mode, (), (const, override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, open, (const std::weak_ptr<ILevel>&, ILevel::OpenMode), (override));
            MOCK_METHOD(void, select_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, select_sector, (const std::weak_ptr<ISector>&), (override));
            MOCK_METHOD(void, select_waypoint, (const std::weak_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, select_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, select_flyby_node, (const std::weak_ptr<IFlybyNode>&), (override));
            MOCK_METHOD(void, select_static_mesh, (const std::weak_ptr<IStaticMesh>&), (override));
            MOCK_METHOD(void, set_camera_mode, (ICamera::Mode), (override));
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
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
        };
    }
}
