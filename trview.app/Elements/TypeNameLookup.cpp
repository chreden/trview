#include "TypeNameLookup.h"
#include <external/nlohmann/json.hpp>
#include <trview/ResourceHelper.h>
#include <trview/resource.h>


namespace trview
{
    TypeNameLookup::TypeNameLookup(trlevel::LevelVersion version)
    {
        Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");

        auto contents = std::string(type_list.data, type_list.data + type_list.size);
        auto json = nlohmann::json::parse(contents.begin(), contents.end());

        std::string game_name;
        switch (version)
        {
        case trlevel::LevelVersion::Tomb1:
            game_name = "tr1";
            break;
        case trlevel::LevelVersion::Tomb2:
            game_name = "tr2";
            break;
        case trlevel::LevelVersion::Tomb3:
            game_name = "tr3";
            break;
        case trlevel::LevelVersion::Tomb4:
            game_name = "tr4";
            break;
        case trlevel::LevelVersion::Tomb5:
            game_name = "tr5";
            break;
        default:
            return;
        }

        for (const auto& element : json["games"][game_name])
        {
            auto name = element.at("name").get<std::string>();
            _type_names.insert({ element.at("id").get<uint32_t>(), std::wstring(name.begin(), name.end()) });
        }
    }

    std::wstring TypeNameLookup::lookup_type_name(uint32_t type_id) const
    {
        const auto found = _type_names.find(type_id);
        if (found == _type_names.end())
        {
            return std::to_wstring(type_id);
        }
        return found->second;
    }
}
