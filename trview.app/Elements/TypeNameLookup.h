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
        virtual std::wstring lookup_type_name(trlevel::LevelVersion level_version, uint32_t type_id) const override;
        virtual bool is_pickup(trlevel::LevelVersion level_version, uint32_t type_id) const override;
    private:
        struct Type
        {
            std::wstring name;
            bool pickup{ false };
        };

        std::unordered_map<trlevel::LevelVersion, std::unordered_map<uint32_t, Type>> _type_names;
    };
}
