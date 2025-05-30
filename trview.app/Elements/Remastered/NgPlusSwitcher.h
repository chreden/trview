#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <trlevel/LevelVersion.h>

#include "INgPlusSwitcher.h"

#include "../IItem.h"

namespace trview
{
    class NgPlusSwitcher final : public INgPlusSwitcher
    {
    public:
        NgPlusSwitcher(const IItem::EntitySource& item_source);
        virtual ~NgPlusSwitcher() = default;
        std::unordered_map<uint16_t, std::shared_ptr<IItem>> create_for_level(const std::shared_ptr<ILevel>& level, const trlevel::ILevel& tr_level, const IModelStorage& model_storage) const override;
    private:
        std::unordered_map<trlevel::LevelVersion,
            std::unordered_map<std::string,
            std::unordered_map<uint16_t, int16_t>>> _item_mapping;
        IItem::EntitySource _item_source;
    };
}
