#pragma once

#include <trview.common/Colour.h>
#include "../Elements/Types.h"

namespace trview
{
    class MapColours final
    {
    public:
        enum class Special
        {
            Default,
            NoSpace,
            RoomAbove,
            RoomBelow
        };

        std::unordered_map<uint16_t, Colour> colours() const;
        Colour colour_from_flag(uint16_t flag) const;
        Colour colour_from_flags_field(uint16_t flags) const;
        Colour colour(Special type) const;
        void set_colour(uint16_t flag, const Colour& colour);
        void set_colour(Special type, const Colour& colour);
    private:
        std::unordered_map<uint16_t, Colour> _override_colours;
        std::unordered_map<Special, Colour> _override_special_colours;
    };
}
