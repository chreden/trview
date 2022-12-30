#pragma once

#include "../../Windows/CameraSink/ICameraSinkWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockCameraSinkWindowManager : public ICameraSinkWindowManager
        {
            MockCameraSinkWindowManager();
            virtual ~MockCameraSinkWindowManager();
            MOCK_METHOD(void, set_camera_sinks, (const std::vector<std::weak_ptr<ICameraSink>>&), (override));
            MOCK_METHOD(std::weak_ptr<ICameraSinkWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, set_room, (uint32_t), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
        };
    }
}
