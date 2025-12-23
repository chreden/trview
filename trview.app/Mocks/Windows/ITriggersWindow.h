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
            MOCK_METHOD(void, clear_selected_trigger, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, selected_trigger, (), (const, override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
