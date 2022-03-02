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

        // Also need:
        // Fallback
        // NO-SPACE
        // Down
        // Up
    }

    std::unordered_map<uint16_t, Colour> MapColours::colours() const
    {
        std::unordered_map<uint16_t, Colour> result;
        for (const auto& color : default_colours)
        {
            uint16_t key = color.first;
            auto override_colour = _override_colours.find(key);
            if (override_colour != _override_colours.end())
            {
                result[key] = override_colour->second;
            }
            else
            {
                result[key] = color.second;
            }
        }
        return result;
    }

    Colour MapColours::colour_from_flag(uint16_t flag) const
    {
        auto override_colour = _override_colours.find(flag);
        if (override_colour != _override_colours.end())
        {
            return override_colour->second;
        }
        else
        {
            return default_colours[flag];
        }
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
                uint16_t key = color.first;
                if ((key & flags)
                    && (key < minimum_flag_enabled || minimum_flag_enabled == -1)
                    && (key < SectorFlag::ClimbableUp || key > SectorFlag::ClimbableLeft)) // climbable flag handled separately
                {
                    minimum_flag_enabled = key;
                    auto override_colour = _override_colours.find(key);
                    if (override_colour != _override_colours.end())
                    {
                        draw_color = override_colour->second;
                    }
                    else
                    {
                        draw_color = color.second;
                    }
                }
            }
        }

        return draw_color;
    }

    void MapColours::set_colour(uint16_t flag, const Colour& colour)
    {
        _override_colours[flag] = colour;
    }
}
