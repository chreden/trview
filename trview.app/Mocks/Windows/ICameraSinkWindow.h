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
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
