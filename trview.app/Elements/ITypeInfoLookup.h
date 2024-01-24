#pragma once

#include <optional>
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
        virtual std::string lookup_type_name(trlevel::LevelVersion level_version, uint32_t type_id, uint16_t flags) const = 0;
        virtual bool is_pickup(trlevel::LevelVersion level_version, uint32_t type_id) const = 0;
        virtual std::optional<TypeInfo> lookup(trlevel::LevelVersion level_version, uint32_t type_id, int16_t flags) const = 0;
    };
}
