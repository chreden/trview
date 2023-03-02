#pragma once

#include "IPlugin.h"

namespace trview
{
    struct Plugin final : public IPlugin
    {
        explicit Plugin(const std::vector<uint8_t>& bytes);
        virtual ~Plugin() = default;
    };
}
