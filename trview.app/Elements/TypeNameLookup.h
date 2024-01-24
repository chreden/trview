#pragma once

#include "ITypeNameLookup.h"
#include <unordered_map>
#include <unordered_set>

namespace trview
{
    class TypeNameLookup : public ITypeNameLookup
    {
    public:
        explicit TypeNameLookup(const std::string& type_name_json);
        virtual ~TypeNameLookup() = default;
        virtual std::string lookup_type_name(trlevel::LevelVersion level_version, uint32_t type_id, uint16_t flags) const override;
        virtual bool is_pickup(trlevel::LevelVersion level_version, uint32_t type_id) const override;
    private:
        struct Type
        {
            std::string name;
            bool pickup{ false };
            std::unordered_set<std::string> categories;
        };

        std::unordered_map<trlevel::LevelVersion, std::unordered_map<uint32_t, Type>> _type_names;
    };
}
