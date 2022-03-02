#include "MapColours.h"
#include "../Elements/Types.h"

namespace trview
{
    namespace
    {
        std::map<uint16_t, Colour> default_colours = {
            { SectorFlag::Portal, { 0.0f, 0.0f, 0.0f } },
            { SectorFlag::Wall, { 0.4f, 0.4f, 0.4f } },
            { SectorFlag::Trigger, { 1.0f, 0.3f, 0.7f } },
            { SectorFlag::Death, { 0.9f, 0.1f, 0.1f } },
            { SectorFlag::MinecartLeft, { 0.0f, 0.9f, 0.9f } },
            { SectorFlag::MinecartRight, { 0.0f, 0.9f, 0.9f } },
            { SectorFlag::MonkeySwing, { 0.9f, 0.9f, 0.4f } },
            { SectorFlag::ClimbableUp, { 0.6f, 0.0f, 0.9f, 0.0f } },
            { SectorFlag::ClimbableDown, { 0.6f, 0.0f, 0.9f, 0.0f } },
            { SectorFlag::ClimbableRight, { 0.6f, 0.0f, 0.9f, 0.0f } },
            { SectorFlag::ClimbableLeft, { 0.6f, 0.0f, 0.9f, 0.0f } },
        };
    }

    Colour MapColours::colour_from_flag(uint16_t flag) const
    {
        return default_colours.find(flag)->second;
    }

    Colour MapColours::colour_from_flags_field(uint16_t flags) const
    {
        // Firstly get the colour for the tile 
        // To determine the base colour we order the floor functions by the *minimum* enabled flag (ranked by order asc)
        int minimum_flag_enabled = -1;
        Colour draw_color = { 0.0f, 0.7f, 0.7f }; // fallback 

        if (!(flags & SectorFlag::Portal) && (flags & SectorFlag::Wall && flags & SectorFlag::FloorSlant)) // is it no-space?
        {
            return { 0.2f, 0.2f, 0.9f };
        }
        else
        {
            for (const auto& color : default_colours)
            {
                if ((color.first & flags)
                    && (color.first < minimum_flag_enabled || minimum_flag_enabled == -1)
                    && (color.first < SectorFlag::ClimbableUp || color.first > SectorFlag::ClimbableLeft)) // climbable flag handled separately
                {
                    minimum_flag_enabled = color.first;
                    draw_color = color.second;
                }
            }
        }

        return draw_color;
    }
}
