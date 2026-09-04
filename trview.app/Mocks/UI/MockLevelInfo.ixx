module;

#include <gmock/gmock.h>

export module trview.app:MockLevelInfo;

import :ILevelInfo;

namespace trview
{
    namespace mocks
    {
        export struct MockLevelInfo : public ILevelInfo
        {
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_level, (const std::weak_ptr<ILevel>&), (override));
        };
    }
}
