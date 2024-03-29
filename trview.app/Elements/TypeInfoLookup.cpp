#include "TypeInfoLookup.h"
#include <trview.common/Json.h>
#include "IItem.h"

using namespace trlevel;

namespace trview
{
    namespace
    {
        const std::unordered_map<int, std::string> mutant_names
        {
            { 20, "Winged" },
            { 21, "Shooter" },
            { 22, "Grounded" },
            { 23, "Centaur" },
            { 34, "Torso" }
        };

        std::string mutant_name(uint16_t flags)
        {
            auto mutant_name = mutant_names.find(mutant_egg_contents((flags & 0x3E00) >> 9));
            return std::format("Mutant Egg ({})", mutant_name == mutant_names.end() ? "Winged" : mutant_name->second);
        }
    }

    ITypeInfoLookup::~ITypeInfoLookup()
    {
    }

    TypeInfoLookup::TypeInfoLookup(const std::string& type_name_json)
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

            std::unordered_map<uint32_t, TypeInfo> type_names;
            for (const auto& element : json["games"][game_name])
            {
                auto name = element.at("name").get<std::string>();
                type_names.insert({ element.at("id").get<uint32_t>(), 
                    {
                        name,
                        read_attribute<std::unordered_set<std::string>>(element, "categories")
                    } });
            }
            _type_names.insert({ version, type_names });
        };

        load_game_types(LevelVersion::Tomb1);
        load_game_types(LevelVersion::Tomb2);
        load_game_types(LevelVersion::Tomb3);
        load_game_types(LevelVersion::Tomb4);
        load_game_types(LevelVersion::Tomb5);
    }

    TypeInfo TypeInfoLookup::lookup(trlevel::LevelVersion level_version, uint32_t type_id, int16_t flags) const
    {
        const auto& game_types = _type_names.find(level_version);
        if (game_types == _type_names.end())
        {
            return { .name = std::to_string(type_id) };
        }

        const auto found_type = game_types->second.find(type_id);
        if (found_type == game_types->second.end())
        {
            return { .name = std::to_string(type_id) };
        }

        TypeInfo result = found_type->second;
        if (level_version == LevelVersion::Tomb1 && is_mutant_egg(type_id))
        {
            result.name = mutant_name(flags);
        }
        return result;
    }
}
