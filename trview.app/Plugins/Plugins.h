#pragma once

#include "IPlugins.h"

namespace trview
{
    struct Plugins final : public IPlugins
    {
        virtual ~Plugins() = default;
    };
}
