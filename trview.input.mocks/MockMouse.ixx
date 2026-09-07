module;

#include <gmock/gmock.h>

export module trview.input.mocks:MockMouse;

import trview.input;

namespace trview
{
    namespace input
    {
        namespace mocks
        {
            export struct MockMouse : public IMouse
            {
                MOCK_METHOD(long, x, (), (const, override));
                MOCK_METHOD(long, y, (), (const, override));
            };
        }
    }
}
