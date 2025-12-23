#pragma once

#include "../../Windows/ILightsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockLightsWindow : public ILightsWindow
        {
            MockLightsWindow();
            virtual ~MockLightsWindow();
            MOCK_METHOD(void, clear_selected_light, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}
