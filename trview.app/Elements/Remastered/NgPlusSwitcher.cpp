#include "NgPlusSwitcher.h"
#include "../../Resources/resource.h"
#include <trview.common/Resources.h>
#include "../ILevel.h"
#include <trlevel/ILevel.h>

using namespace trlevel;

namespace trview
{
    const std::unordered_map<std::string, LevelVersion> version_mapping
    {
        { "tr1", LevelVersion::Tomb1 },
        { "tr2", LevelVersion::Tomb2 },
        { "tr3", LevelVersion::Tomb3 },
        { "tr4", LevelVersion::Tomb4 },
        { "tr5", LevelVersion::Tomb5 }
    };

    INgPlusSwitcher::~INgPlusSwitcher()
    {
    }

    NgPlusSwitcher::NgPlusSwitcher(const IItem::EntitySource& item_source)
        : _item_source(item_source)
    {
        Resource type_list = get_resource_memory(IDR_NGPLUS, L"TEXT");
        std::string ngplus_json = { type_list.data, type_list.data + type_list.size };
        auto json = nlohmann::json::parse(ngplus_json.begin(), ngplus_json.end(), nullptr, true, true, true);
        for (auto& [key, value] : json["games"].items())
        {
            const auto version = version_mapping.find(key);
            if (version == version_mapping.end())
            {
                continue;
            }

            std::unordered_map<std::string, std::unordered_map<uint16_t, int16_t>> level_maps;
            std::unordered_map<std::string, std::vector<ReplacementEntity>> level_extras_maps;
            for (auto& [level_key, level_value] : value.items())
            {
                if (level_value.contains("adds"))
                {
                    const auto adds = level_value["adds"];
                    std::vector<ReplacementEntity> extra_entries;
                    for (auto& [_, item_value] : adds.items())
                    {
                        ReplacementEntity new_entity;
                        new_entity.ng_plus = item_value["NgPlus"].get<bool>();
                        new_entity.index = item_value["Index"].get<uint16_t>();
                        new_entity.entity.TypeID = item_value["TypeID"].get<uint16_t>();
                        new_entity.entity.Room = item_value["Room"].get<uint16_t>();
                        new_entity.entity.x = item_value["X"].get<int32_t>();
                        new_entity.entity.y = item_value["Y"].get<int32_t>();
                        new_entity.entity.z = item_value["Z"].get<int32_t>();
                        extra_entries.push_back(new_entity);
                    }
                    level_extras_maps[level_key] = extra_entries;
                }

                const auto swaps = level_value["swaps"];
                std::unordered_map<uint16_t, int16_t> entries;
                for (auto& [_, item_value] : swaps.items())
                {
                    entries[item_value["item"].get<uint16_t>()] = item_value["type"].get<uint16_t>();
                }
                level_maps[level_key] = entries;
            }
            _extra_item_mapping[version->second] = level_extras_maps;
            _item_mapping[version->second] = level_maps;
        }
    }

    std::vector<std::shared_ptr<IItem>> NgPlusSwitcher::extras_for_level(
        const std::shared_ptr<ILevel>& level,
        const trlevel::ILevel& tr_level,
        const IModelStorage& model_storage) const
    {
        const auto game_mapping = _extra_item_mapping.find(level->version());
        if (game_mapping == _extra_item_mapping.end())
        {
            return {};
        }

        const auto level_mapping = game_mapping->second.find(level->name());
        if (level_mapping == game_mapping->second.end())
        {
            return {};
        }

        std::vector<std::shared_ptr<IItem>> results;
        for (const auto& entry : level_mapping->second)
        {
            auto room = level->room(entry.entity.Room);
            auto item = _item_source(tr_level, entry.entity, entry.index, {}, model_storage, level, room);
            item->set_remastered_extra(true);
            if (entry.ng_plus)
            {
                item->set_ng_plus(entry.ng_plus);
            }
            results.push_back(item);
        }

        return results;
    }

    std::unordered_map<uint16_t, std::shared_ptr<IItem>> NgPlusSwitcher::create_for_level(const std::shared_ptr<ILevel>& level, const trlevel::ILevel& tr_level, const IModelStorage& model_storage) const
    {
        const auto game_mapping = _item_mapping.find(level->version());
        if (game_mapping == _item_mapping.end())
        {
            return {};
        }

        const auto level_mapping = game_mapping->second.find(level->name());
        if (level_mapping == game_mapping->second.end())
        {
            return {};
        }

        std::unordered_map<uint16_t, std::shared_ptr<IItem>> results;
        for (const auto& entry : level_mapping->second)
        {
            const auto existing_item = level->item(entry.first).lock();
            if (!existing_item)
            {
                continue;
            }

            existing_item->set_ng_plus(false);

            if (entry.second != -1)
            {
                trlevel::tr2_entity entity
                {
                    .TypeID = static_cast<int16_t>(existing_item->type_id()),
                    .Room = static_cast<int16_t>(room_number(existing_item->room())),
                    .x = static_cast<int32_t>(existing_item->position().x * trlevel::Scale),
                    .y = static_cast<int32_t>(existing_item->position().y * trlevel::Scale),
                    .z = static_cast<int32_t>(existing_item->position().z * trlevel::Scale),
                    .Angle = static_cast<int16_t>(existing_item->angle()),
                    .Intensity1 = 0,
                    .Intensity2 = 0,
                    .Flags = existing_item->activation_flags()
                };

                entity.TypeID = entry.second;
                auto item = _item_source(tr_level, entity, entry.first, existing_item->triggers(), model_storage, level, existing_item->room());
                item->set_ng_plus(true);
                item->set_categories(existing_item->categories());
                results[entry.first] = item;
            }
            else
            {
                results[entry.first] = nullptr;
            }
        }

        return results;
    }
}
