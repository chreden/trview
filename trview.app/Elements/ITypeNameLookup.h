#pragma once

#include <cstdint>
#include <string>
#include <trlevel/LevelVersion.h>

namespace trview
{
    struct ITypeNameLookup
    {
    public:
        virtual ~ITypeNameLookup() = 0;
        virtual std::wstring lookup_type_name(trlevel::LevelVersion level_version, uint32_t type_id) const = 0;
        virtual bool is_pickup(trlevel::LevelVersion level_version, uint32_t type_id) const = 0;
    };
}
