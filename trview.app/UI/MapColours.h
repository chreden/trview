#pragma once

#include <trview.common/Colour.h>
#include "../Elements/Types.h"
#include <external/nlohmann/json.hpp>

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
            RoomBelow,
            GeometryWall
        };

        std::unordered_map<SectorFlag, Colour> override_colours() const;
        std::unordered_map<Special, Colour> special_colours() const;
        void clear_colour(SectorFlag flag);
        void clear_colour(Special flag);
        Colour colour(SectorFlag flag) const;
        Colour colour_from_flags_field(SectorFlag flags) const;
        Colour colour(Special type) const;
        void set_colour(SectorFlag flag, const Colour& colour);
        void set_colour(Special type, const Colour& colour);
    private:
        std::unordered_map<SectorFlag, Colour> _override_colours;
        std::unordered_map<Special, Colour> _override_special_colours;
    };

    void to_json(nlohmann::json& json, const MapColours& colours);
    void from_json(const nlohmann::json& json, MapColours& colours);
}
