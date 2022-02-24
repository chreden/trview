#pragma once

#include "../../Windows/IShell.h"

namespace trview
{
    namespace mocks
    {
        struct MockShell : public IShell
        {
            virtual ~MockShell() = default;
            MOCK_METHOD(void, open, (const std::wstring&), (override));
        };
    }
}
