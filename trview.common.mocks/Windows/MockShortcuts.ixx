module;

#include <gmock/gmock.h>

export module trview.common.mocks:MockShortcuts;

import trview.common;
import std;

namespace trview
{
    namespace mocks
    {
        export struct MockShortcuts : public IShortcuts
        {
            MOCK_METHOD(Event<>&, add_shortcut, (bool, uint16_t));
            MOCK_METHOD(std::vector<Shortcut>, shortcuts, (), (const));
        };
    }
}