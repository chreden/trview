export module trview.app:IFilterable;

import std.compat;

namespace trview
{
    export struct IFilterable
    {
        virtual ~IFilterable() = 0;
        virtual int32_t filterable_index() const = 0;
    };
}
