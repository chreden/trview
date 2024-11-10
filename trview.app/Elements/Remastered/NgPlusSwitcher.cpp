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
        auto json = nlohmann::json::parse(ngplus_json.begin(), ngplus_json.end());
        for (auto& [key, value] : json["games"].items())
        {
            const auto version = version_mapping.find(key);
            if (version == version_mapping.end())
            {
                continue;
            }

            std::unordered_map<std::string, std::unordered_map<uint16_t, uint16_t>> level_maps;
            for (auto& [level_key, level_value] : value.items())
            {
                std::unordered_map<uint16_t, uint16_t> entries;
                for (auto& [_, item_value] : level_value.items())
                {
                    entries[item_value["item"].get<uint16_t>()] = item_value["type"].get<uint16_t>();
                }
                level_maps[level_key] = entries;
            }
            _item_mapping[version->second] = level_maps;
        }
    }

    INgPlusSwitcher::SwapSet NgPlusSwitcher::create_for_level(const std::shared_ptr<ILevel>& level, const trlevel::ILevel& tr_level, const IMeshStorage& mesh_storage) const
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

        SwapSet swapset;
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
                auto entity = tr_level.get_entity(entry.first);
                entity.TypeID = entry.second;
                auto item = _item_source(tr_level, entity, entry.first, existing_item->triggers(), mesh_storage, level, existing_item->room());
                item->set_ng_plus(true);
                swapset.items[entry.first] = item;
            }
            else
            {
                swapset.items[entry.first] = nullptr;
            }
        }

        return swapset;
    }
}
