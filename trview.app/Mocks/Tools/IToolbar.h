#pragma once

#include "../../Tools/IToolbar.h"

namespace trview
{
    namespace mocks
    {
        struct MockToolbar : public IToolbar
        {
            MockToolbar();
            ~MockToolbar();
            MOCK_METHOD(void, add_tool, (const std::string&), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}