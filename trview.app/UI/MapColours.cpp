#include "MapColours.h"
#include "../Elements/Types.h"
#include <trview.common/JsonSerializers.h>
#include "../Elements/ISector.h"

namespace trview
{
    namespace
    {
        const std::map<SectorFlag, Colour> default_colours = {
            { SectorFlag::Portal, { 0.0f, 0.0f, 0.0f } },
            { SectorFlag::SpecialWall, { 0.0f, 0.5f, 0.0f } },
            { SectorFlag::Wall, { 0.4f, 0.4f, 0.4f } },
            { SectorFlag::Trigger, { 1.0f, 0.3f, 0.7f } },
            { SectorFlag::Death, { 0.9f, 0.1f, 0.1f } },
            { SectorFlag::MinecartLeft, { 0.0f, 0.9f, 0.9f } },
            { SectorFlag::MinecartRight, { 0.0f, 0.9f, 0.9f } },
            { SectorFlag::MonkeySwing, { 0.9f, 0.9f, 0.4f } },
            { SectorFlag::ClimbableNorth, { 0.6f, 0.0f, 0.9f, 0.0f } },
            { SectorFlag::ClimbableSouth, { 0.6f, 0.0f, 0.9f, 0.0f } },
            { SectorFlag::ClimbableEast, { 0.6f, 0.0f, 0.9f, 0.0f } },
            { SectorFlag::ClimbableWest, { 0.6f, 0.0f, 0.9f, 0.0f } }
        };

        const std::unordered_map<MapColours::Special, Colour> default_special_colours = {
            { MapColours::Special::Default, { 0.0f, 0.7f, 0.7f } },
            { MapColours::Special::NoSpace, { 0.2f, 0.2f, 0.9f } },
            { MapColours::Special::RoomAbove, { 0.0f, 0.0f, 0.0f } },
            { MapColours::Special::RoomBelow, { 0.6f, 0.0f, 0.0f, 0.0f } },
            { MapColours::Special::GeometryWall, { 0.0f, 0.6f, 0.15f } }
        };
    }

    std::unordered_map<SectorFlag, Colour> MapColours::override_colours() const
    {
        return _override_colours;
    }

    std::unordered_map<MapColours::Special, Colour> MapColours::special_colours() const
    {
        return _override_special_colours;
    }

    void MapColours::clear_colour(SectorFlag flag)
    {
        _override_colours.erase(flag);
    }
    
    void MapColours::clear_colour(Special flag)
    {
        _override_special_colours.erase(flag);
    }

    Colour MapColours::colour(SectorFlag flag) const
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

    Colour MapColours::colour_from_flags_field(SectorFlag flags) const
    {
        // Firstly get the colour for the tile 
        // To determine the base colour we order the floor functions by the *minimum* enabled flag (ranked by order asc)
        int minimum_flag_enabled = -1;
        Colour draw_color = colour(Special::Default);

        if (is_no_space(flags))
        {
            return colour(Special::NoSpace);
        }
        else
        {
            for (const auto& color : default_colours)
            {
                auto key = color.first;
                if (has_flag(flags, key)
                    && (static_cast<int>(key) < minimum_flag_enabled || minimum_flag_enabled == -1)
                    && (key < SectorFlag::ClimbableNorth || key > SectorFlag::ClimbableWest)) // climbable flag handled separately
                {
                    minimum_flag_enabled = static_cast<int>(key);
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

    void MapColours::set_colour(SectorFlag flag, const Colour& colour)
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
        auto overrides = read_attribute<std::map<SectorFlag, Colour>>(json, "colours");
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
