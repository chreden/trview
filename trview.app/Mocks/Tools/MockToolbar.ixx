module;

#include <gmock/gmock.h>

export module trview.app:MockToolbar;

import :IToolbar;

namespace trview
{
    namespace mocks
    {
        export struct MockToolbar : public IToolbar
        {
            MOCK_METHOD(void, add_tool, (const std::string&), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}