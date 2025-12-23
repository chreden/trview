#pragma once

#include "../../Windows/Sounds/ISoundsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockSoundsWindowManager : public ISoundsWindowManager
        {
        public:
            MockSoundsWindowManager();
            virtual ~MockSoundsWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<ISoundsWindow>, create_window, (), (override));
        };
    }
}
