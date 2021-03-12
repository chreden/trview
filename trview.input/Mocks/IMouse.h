#pragma once

#include "../IMouse.h"

namespace trview
{
    namespace input
    {
        namespace mocks
        {
            class MockMouse final : public IMouse
            {
            public:
                virtual ~MockMouse() = default;
                MOCK_METHOD(long, x, (), (const));
                MOCK_METHOD(long, y, (), (const));
            };
        }
    }
}
