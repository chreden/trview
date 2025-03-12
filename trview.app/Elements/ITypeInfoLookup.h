#pragma once

#include <cstdint>
#include <string>
#include <trlevel/LevelVersion.h>
#include "TypeInfo.h"

namespace trview
{
    struct ITypeInfoLookup
    {
    public:
        virtual ~ITypeInfoLookup() = 0;
        virtual TypeInfo lookup(trlevel::PlatformAndVersion level_version, uint32_t type_id, int16_t flags) const = 0;
    };
}
