#pragma once

#include "../../Windows/ITriggersWindow.h"

namespace trview
{
    class MockTriggersWindow : public ITriggersWindow
    {
    public:
        MOCK_METHOD(void, clear_selected_trigger, ());
        MOCK_METHOD(void, render, (const graphics::IDevice&, bool));
        MOCK_METHOD(std::optional<const Trigger*>, selected_trigger, (), (const));
        MOCK_METHOD(void, set_current_room, (uint32_t));
        MOCK_METHOD(void, set_items, (const std::vector<Item>&));
        MOCK_METHOD(void, set_selected_trigger, (const Trigger* const));
        MOCK_METHOD(void, set_triggers, (const std::vector<Trigger*>&));
        MOCK_METHOD(void, update_triggers, (const std::vector<Trigger*>&));
        MOCK_METHOD(ui::Control*, root_control, (), (const));
    };
}
