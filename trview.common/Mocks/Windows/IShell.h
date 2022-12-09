#pragma once

#include "../../Windows/IShell.h"

namespace trview
{
    namespace mocks
    {
        struct MockShell : public IShell
        {
            MockShell();
            virtual ~MockShell();
            MOCK_METHOD(void, open, (const std::wstring&), (override));
        };
    }
}
