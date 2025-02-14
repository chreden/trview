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
            MOCK_METHOD(void, add_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(std::weak_ptr<ICameraSinkWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
        };
    }
}
