#pragma once

#include "../../Windows/ILauWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockLauWindow final : public ILauWindow
        {
            virtual ~MockLauWindow() = default;
            MOCK_METHOD(void, render, (bool), (override));
        };
    }
}
