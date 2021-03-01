#pragma once

#include "ILevel.h"

namespace trlevel
{
    struct ILevelLoader
    {
        virtual ~ILevelLoader() = 0;
        virtual std::unique_ptr<ILevel> load_level(const std::string& filename) const = 0;
    };
}
