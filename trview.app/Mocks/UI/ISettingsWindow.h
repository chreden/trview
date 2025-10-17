#pragma once

#include "../../UI/ISettingsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockSettingsWindow : public ISettingsWindow
        {
            MockSettingsWindow();
            virtual ~MockSettingsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, toggle_visibility, (), (override));
        };
    }
}
