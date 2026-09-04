module;

#include <gmock/gmock.h>

export module trview.app:MockSettingsWindow;

import :ISettingsWindow;

namespace trview
{
    namespace mocks
    {
        export struct MockSettingsWindow : public ISettingsWindow
        {
            MockSettingsWindow();
            virtual ~MockSettingsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, toggle_visibility, (), (override));
        };
    }
}
