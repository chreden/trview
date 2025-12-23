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
            MOCK_METHOD(std::weak_ptr<ITexturesWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
