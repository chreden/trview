#pragma once

namespace trview
{
    struct ILevel;
    struct ILevelNameLookup
    {
        virtual ~ILevelNameLookup() = 0;
        virtual std::string lookup(const std::weak_ptr<ILevel>& level) const = 0;
    };
}
