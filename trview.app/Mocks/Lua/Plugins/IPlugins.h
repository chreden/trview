#pragma once

#include "../../Lua/Plugins/IPlugins.h"

namespace trview
{
    namespace mocks
    {
        struct MockPlugins : public IPlugins
        {
            virtual ~MockPlugins() = default;
            MOCK_METHOD(void, load, (), (override));
            MOCK_METHOD(void, initialise, (), (override));
            MOCK_METHOD(void, render_ui, (), (override));
        };
    }
}
