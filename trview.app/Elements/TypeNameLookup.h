#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <trlevel/trlevel.h>

namespace trview
{
    class TypeNameLookup
    {
    public:
        TypeNameLookup(trlevel::LevelVersion version);

        std::wstring lookup_type_name(uint32_t type_id) const;
    private:
        std::unordered_map<uint32_t, std::wstring> _type_names;
    };
}
