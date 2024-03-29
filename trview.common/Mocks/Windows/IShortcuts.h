#pragma once

#include "../../Windows/IShortcuts.h"

namespace trview
{
    namespace mocks
    {
        struct MockShortcuts : public IShortcuts
        {
            MockShortcuts();
            virtual ~MockShortcuts();
            MOCK_METHOD(Event<>&, add_shortcut, (bool, uint16_t));
            MOCK_METHOD(std::vector<Shortcut>, shortcuts, (), (const));
        };
    }
}