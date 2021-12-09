#pragma once

#include "Control.h"

namespace trview
{
    namespace ui
    {
        struct ILoader
        {
            virtual ~ILoader() = 0;
            virtual std::unique_ptr<Control> load_from_resource(uint32_t resource_id) const = 0;
        };
    }
}

