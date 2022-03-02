#pragma once

#include <trview.common/Colour.h>
#include "../Elements/Types.h"

namespace trview
{
    class MapColours final
    {
    public:
        std::unordered_map<uint16_t, Colour> colours() const;
        Colour colour_from_flag(uint16_t flag) const;
        Colour colour_from_flags_field(uint16_t flags) const;
        void set_colour(uint16_t flag, const Colour& colour);
    private:
        std::unordered_map<uint16_t, Colour> _override_colours;
    };
}
