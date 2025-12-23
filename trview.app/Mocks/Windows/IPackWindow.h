#pragma once

#include "../../Windows/Pack/IPackWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockPackWindow : public IPackWindow
        {
            MockPackWindow();
            virtual ~MockPackWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}