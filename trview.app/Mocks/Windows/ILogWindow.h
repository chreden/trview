#pragma once

#include "../../Windows/Log/ILogWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockLogWindow : public ILogWindow
        {
            virtual ~MockLogWindow() = default;
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}
