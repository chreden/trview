#pragma once

namespace trlevel
{
    struct ILevel;
}

namespace trview
{
    struct IItem;
    struct ILevel;
    struct IModelStorage;

    struct INgPlusSwitcher
    {
        virtual ~INgPlusSwitcher() = 0;
        virtual std::vector<std::shared_ptr<IItem>> extras_for_level(
            const std::shared_ptr<ILevel>& level,
            const trlevel::ILevel& tr_level,
            const IModelStorage& model_storage) const = 0;
        virtual std::unordered_map<uint16_t, std::shared_ptr<IItem>> create_for_level(
            const std::shared_ptr<ILevel>& level,
            const trlevel::ILevel& tr_level,
            const IModelStorage& model_storage) const = 0;
    };
}
