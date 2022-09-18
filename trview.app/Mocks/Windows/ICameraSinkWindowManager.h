#pragma once

#include "../../Windows/CameraSink/ICameraSinkWindowManager.h"

namespace trview
{
    struct MockCameraSinkWindowManager : public ICameraSinkWindowManager
    {
        virtual ~MockCameraSinkWindowManager() = default;
        MOCK_METHOD(void, set_camera_sinks, (const std::vector<std::weak_ptr<ICameraSink>>&), (override));
        MOCK_METHOD(std::weak_ptr<ICameraSinkWindow>, create_window, (), (override));
        MOCK_METHOD(void, render, (), (override));
    };
}
