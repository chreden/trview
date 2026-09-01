export module trview.app:INgPlusSwitcher;

import std;
import std.compat;

import trlevel;

import :IItem;
import :ILevel;
import :IModelStorage;

namespace trview
{
    export struct INgPlusSwitcher
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
