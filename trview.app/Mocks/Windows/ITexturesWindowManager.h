#pragma once

#include "../../Windows/Textures/ITexturesWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockTexturesWindowManager : public ITexturesWindowManager
        {
            MockTexturesWindowManager();
            virtual ~MockTexturesWindowManager();
            MOCK_METHOD(void, add_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(std::weak_ptr<ITexturesWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
