module;

#include <gmock/gmock.h>

export module trview.common:MockShell;

import :IShell;

namespace trview
{
    namespace mocks
    {
        export struct MockShell : public IShell
        {
            MockShell();
            virtual ~MockShell();
            MOCK_METHOD(void, open, (const std::wstring&), (override));
        };
    }
}
