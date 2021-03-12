#pragma once

#include <trview.common/Windows/IShortcuts.h>

namespace trview
{
    namespace mocks
    {
        class MockShortcuts : public IShortcuts
        {
        public:
            virtual ~MockShortcuts() = default;
            MOCK_METHOD(Event<>&, add_shortcut, (bool, uint16_t));
            MOCK_METHOD(std::vector<Shortcut>, shortcuts, (), (const));
        };
    }
}