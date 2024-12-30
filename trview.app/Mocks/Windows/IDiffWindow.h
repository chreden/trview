#pragma once

#include "../../Windows/Diff/IDiffWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockDiffWindow : public IDiffWindow
        {
            MockDiffWindow();
            virtual ~MockDiffWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}