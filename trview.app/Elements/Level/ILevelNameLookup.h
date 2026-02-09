#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <string>

namespace trlevel
{
    struct ILevel;
}

namespace trview
{
    struct ILevel;
    struct ILevelNameLookup
    {
        struct Name
        {
            std::string name;
            std::optional<int32_t> index;
        };

        virtual ~ILevelNameLookup() = 0;
        virtual std::optional<Name> lookup(const std::weak_ptr<ILevel>& level) const = 0;
        virtual std::optional<Name> lookup(const std::weak_ptr<trlevel::ILevel>& level) const = 0;
        virtual std::vector<int32_t> bonus_items(const std::weak_ptr<ILevel>& level) const = 0;
    };
}
