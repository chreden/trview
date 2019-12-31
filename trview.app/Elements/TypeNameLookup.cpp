#include "TypeNameLookup.h"
#pragma warning(push)
#pragma warning(disable : 4127)
#include <external/nlohmann/json.hpp>
#pragma warning(pop)


using namespace trlevel;

namespace trview
{
    TypeNameLookup::TypeNameLookup(const std::string& type_name_json)
    {
        auto json = nlohmann::json::parse(type_name_json.begin(), type_name_json.end());
        auto load_game_types = [&](LevelVersion version)
        {
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

            std::unordered_map<uint32_t, std::wstring> type_names;
            for (const auto& element : json["games"][game_name])
            {
                auto name = element.at("name").get<std::string>();
                type_names.insert({ element.at("id").get<uint32_t>(), std::wstring(name.begin(), name.end()) });
            }
            _type_names.insert({ version, type_names });
        };

        load_game_types(LevelVersion::Tomb1);
        load_game_types(LevelVersion::Tomb2);
        load_game_types(LevelVersion::Tomb3);
        load_game_types(LevelVersion::Tomb4);
        load_game_types(LevelVersion::Tomb5);
    }

    std::wstring TypeNameLookup::lookup_type_name(LevelVersion level_version, uint32_t type_id) const
    {
        const auto& game_types = _type_names.find(level_version);
        if (game_types == _type_names.end())
        {
            return std::to_wstring(type_id);
        }

        const auto found_type = game_types->second.find(type_id);
        if (found_type == game_types->second.end())
        {
            return std::to_wstring(type_id);
        }
        return found_type->second;
    }
}
