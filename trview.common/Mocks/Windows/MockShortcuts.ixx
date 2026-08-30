module;

#include <gmock/gmock.h>

export module trview.common:MockShortcuts;

import :IShortcuts;
import :Event;
import std;

namespace trview
{
    namespace mocks
    {
        export struct MockShortcuts : public IShortcuts
        {
            MockShortcuts();
            virtual ~MockShortcuts();
            MOCK_METHOD(Event<>&, add_shortcut, (bool, uint16_t));
            MOCK_METHOD(std::vector<Shortcut>, shortcuts, (), (const));
        };
    }
}