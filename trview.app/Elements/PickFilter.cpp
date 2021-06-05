#include "PickFilter.h"

namespace trview
{
    PickFilter operator & (PickFilter left, PickFilter right)
    {
        using T = std::underlying_type_t<PickFilter>;
        return static_cast<PickFilter>(static_cast<T>(left) & static_cast<T>(right));
    }

    PickFilter operator ~ (PickFilter left)
    {
        using T = std::underlying_type_t<PickFilter>;
        return static_cast<PickFilter>(~static_cast<T>(left));
    }

    PickFilter operator | (PickFilter left, PickFilter right)
    {
        using T = std::underlying_type_t<PickFilter>;
        return static_cast<PickFilter>(static_cast<T>(left) | static_cast<T>(right));
    }

    bool has_flag(PickFilter filter, PickFilter flag)
    {
        return (filter & flag) == flag;
    }

    PickFilter filter_flag(PickFilter filter, bool condition)
    {
        return condition ? filter : PickFilter::None;
    }
}
