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
        };
    }
}
