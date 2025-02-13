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
            MOCK_METHOD(void, add_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_selected_light, (const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
