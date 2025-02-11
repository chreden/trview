#pragma once

#include "../../Windows/ITriggersWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockTriggersWindow : public ITriggersWindow
        {
            MockTriggersWindow();
            virtual ~MockTriggersWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
