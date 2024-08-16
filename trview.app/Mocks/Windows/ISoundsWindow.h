#pragma once

#include "../../Windows/Sounds/ISoundsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockSoundsWindow : public ISoundsWindow
        {
            MockSoundsWindow();
            virtual ~MockSoundsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}