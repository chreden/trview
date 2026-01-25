#pragma once

#include <cstdint>

namespace trview
{
    struct IFilterable
    {
        virtual ~IFilterable() = 0;
        virtual int32_t filterable_index() const = 0;
    };
}
