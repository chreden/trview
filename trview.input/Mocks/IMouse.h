#pragma once

#include "../IMouse.h"

namespace trview
{
    namespace input
    {
        namespace mocks
        {
            struct MockMouse : public IMouse
            {
                MockMouse();
                virtual ~MockMouse();
                MOCK_METHOD(long, x, (), (const, override));
                MOCK_METHOD(long, y, (), (const, override));
            };
        }
    }
}
