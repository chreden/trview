#pragma once

#include <trview.common/Colour.h>
#include "../Elements/Types.h"

namespace trview
{
    class MapColours final
    {
    public:
        Colour colour_from_flag(uint16_t flag) const;
        Colour colour_from_flags_field(uint16_t flags) const;
    private:

    };
}
