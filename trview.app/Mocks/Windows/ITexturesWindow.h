#pragma once

#include "../../Windows/Textures/ITexturesWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockTexturesWindow : public ITexturesWindow
        {
            MockTexturesWindow();
            virtual ~MockTexturesWindow();
            MOCK_METHOD(void, add_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}
