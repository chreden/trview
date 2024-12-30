#pragma once

#include "../../Windows/Diff/IDiffWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockDiffWindowManager : public IDiffWindowManager
        {
        public:
            MockDiffWindowManager();
            virtual ~MockDiffWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<IDiffWindow>, create_window, (), (override));
        };
    }
}
