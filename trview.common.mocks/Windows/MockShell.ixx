module;

#include <gmock/gmock.h>

export module trview.common.mocks:MockShell;

import trview.common;

namespace trview
{
    namespace mocks
    {
        export struct MockShell : public IShell
        {
            MOCK_METHOD(void, open, (const std::wstring&), (override));
        };
    }
}
