#pragma once

#include "../../Windows/CameraSink/ICameraSinkWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockCameraSinkWindow : public ICameraSinkWindow
        {
            MockCameraSinkWindow();
            virtual ~MockCameraSinkWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_camera_sinks, (const std::vector<std::weak_ptr<ICameraSink>>&), (override));
            MOCK_METHOD(void, set_flybys, (const std::vector<std::weak_ptr<IFlyby>>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, set_selected_flyby_node, (const std::weak_ptr<IFlybyNode>&), (override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_platform_and_version, (const trlevel::PlatformAndVersion&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
