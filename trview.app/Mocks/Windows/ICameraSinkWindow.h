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
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, set_current_room, (uint32_t), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
        };
    }
}
