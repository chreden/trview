#pragma once

#include "ITypeNameLookup.h"
#include <unordered_map>

namespace trview
{
    class TypeNameLookup : public ITypeNameLookup
    {
    public:
        explicit TypeNameLookup(const std::string& type_name_json);
        virtual ~TypeNameLookup() = default;
        std::wstring lookup_type_name(trlevel::LevelVersion level_version, uint32_t type_id) const override;
    private:
        std::unordered_map<trlevel::LevelVersion, std::unordered_map<uint32_t, std::wstring>> _type_names;
    };
}
