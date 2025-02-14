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
            MOCK_METHOD(void, add_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
        };
    }
}
