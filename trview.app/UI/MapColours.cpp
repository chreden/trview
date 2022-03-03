#include "MapColours.h"
#include "../Elements/Types.h"
#include <trview.common/JsonSerializers.h>

namespace trview
{
    namespace
    {
        const std::map<uint16_t, Colour> default_colours = {
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

        const std::unordered_map<MapColours::Special, Colour> default_special_colours = {
            { MapColours::Special::Default, { 0.0f, 0.7f, 0.7f } },
            { MapColours::Special::NoSpace, { 0.2f, 0.2f, 0.9f } },
            { MapColours::Special::RoomAbove, { 0.0f, 0.0f, 0.0f } },
            { MapColours::Special::RoomBelow, { 0.6f, 0.0f, 0.0f, 0.0f } }
        };
    }

    std::unordered_map<uint16_t, Colour> MapColours::override_colours() const
    {
        return _override_colours;
    }

    std::unordered_map<MapColours::Special, Colour> MapColours::special_colours() const
    {
        return _override_special_colours;
    }

    void MapColours::clear_colour(uint16_t flag)
    {
        _override_colours.erase(flag);
    }
    
    void MapColours::clear_colour(Special flag)
    {
        _override_special_colours.erase(flag);
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
            return default_colours.find(flag)->second;
        }
    }

    Colour MapColours::colour_from_flags_field(uint16_t flags) const
    {
        // Firstly get the colour for the tile 
        // To determine the base colour we order the floor functions by the *minimum* enabled flag (ranked by order asc)
        int minimum_flag_enabled = -1;
        Colour draw_color = colour(Special::Default);

        if (!(flags & SectorFlag::Portal) && (flags & SectorFlag::Wall && flags & SectorFlag::FloorSlant)) // is it no-space?
        {
            return colour(Special::NoSpace);
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

    Colour MapColours::colour(Special type) const
    {
        auto override_special_colour = _override_special_colours.find(type);
        if (override_special_colour != _override_special_colours.end())
        {
            return override_special_colour->second;
        }
        else
        {
            return default_special_colours.find(type)->second;
        }
    }

    void MapColours::set_colour(uint16_t flag, const Colour& colour)
    {
        _override_colours[flag] = colour;
    }

    void MapColours::set_colour(Special type, const Colour& colour)
    {
        _override_special_colours[type] = colour;
    }

    void to_json(nlohmann::json& json, const MapColours& colours)
    {
        json["colours"] = colours.override_colours();
        json["special"] = colours.special_colours();
    }

    void from_json(const nlohmann::json& json, MapColours& colours)
    {
        auto overrides = read_attribute<std::map<uint16_t, Colour>>(json, "colours");
        for (const auto& colour : overrides)
        {
            colours.set_colour(colour.first, colour.second);
        }
        auto specials = read_attribute<std::map<MapColours::Special, Colour>>(json, "special");
        for (const auto& special : specials)
        {
            colours.set_colour(special.first, special.second);
        }
    }
}
