#pragma once

#include <trview.app/Windows/ITriggersWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockTriggersWindowManager final : public ITriggersWindowManager
        {
        public:
            MOCK_METHOD(void, render, (graphics::IDevice&, bool));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, set_triggers, (const std::vector<Trigger*>&));
            MOCK_METHOD(void, set_trigger_visible, (Trigger* trigger, bool));
            MOCK_METHOD(void, set_room, (uint32_t));
            MOCK_METHOD(void, set_selected_trigger, (const Trigger* const));
            MOCK_METHOD(TriggersWindow*, create_window, ());
        };
    }
}