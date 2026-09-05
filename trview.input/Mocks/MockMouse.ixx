module;

#include <gmock/gmock.h>

export module trview.input:MockMouse;

import :IMouse;

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
