#pragma once

#include "../../Plugins/IPlugins.h"

namespace trview
{
    namespace mocks
    {
        struct MockPlugins : public IPlugins
        {
            MockPlugins();
            virtual ~MockPlugins();
            MOCK_METHOD(void, initialise, (IApplication*), (override));
        };
    }
}
