#pragma once

#include <memory>
#include <optional>
#include <string>

namespace trview
{
    struct ILevel;
    struct ILevelNameLookup
    {
        virtual ~ILevelNameLookup() = 0;
        virtual std::optional<std::string> lookup(const std::weak_ptr<ILevel>& level) const = 0;
    };
}
