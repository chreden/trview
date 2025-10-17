#pragma once

#include "../../Windows/Plugins/IPluginsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockPluginsWindow : public IPluginsWindow
        {
            MockPluginsWindow();
            virtual ~MockPluginsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}
