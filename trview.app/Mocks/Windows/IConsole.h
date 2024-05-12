#pragma once

#include "../../Windows/Console/IConsole.h"

namespace trview
{
    namespace mocks
    {
        struct MockConsole : public IConsole
        {
            MockConsole();
            virtual ~MockConsole();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_number, (int32_t number), (override));
        };
    }
}
