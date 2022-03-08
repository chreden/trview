#pragma once

#include "Types.h"
#include "Item.h"

namespace trview
{
    enum class FloordataMeanings
    {
        None,
        Generate
    };

    struct Floordata
    {
        struct Command
        {
            enum class Function : uint16_t
            {
                None,
                Portal,
                FloorSlant,
                CeilingSlant,
                Trigger,
                Death,
                ClimbableWall,
                Triangulation_Floor_NWSE,
                Triangulation_Floor_NESW,
                Triangulation_Ceiling_NW,
                Triangulation_Ceiling_NE,
                Triangulation_Floor_Collision_SW,
                Triangulation_Floor_Collision_NE,
                Triangulation_Floor_Collision_SE,
                Triangulation_Floor_Collision_NW,
                Triangulation_Ceiling_Collision_SW,
                Triangulation_Ceiling_Collision_NE,
                Triangulation_Ceiling_Collision_NW,
                Triangulation_Ceiling_Collision_SE,
                MonkeySwing,
                MinecartLeft_DeferredTrigger,
                MinecartRight_Mapper
            };

            explicit Command(Function type, const std::vector<uint16_t>& data, FloordataMeanings meanings, const std::vector<Item>& items);

            Function type;
            std::vector<uint16_t> data;
            std::vector<std::string> meanings;
        private:
            void create_meanings(const std::vector<Item>& items);
        };

        std::vector<Command> commands;
        uint32_t size() const;
    };

    /// <summary>
    /// Parse the floordata at the specified index.
    /// </summary>
    /// <param name="floordata">The raw floor data.</param>
    /// <param name="index">The index to start at.</param>
    /// <returns>The parsed floor data.</returns>
    Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings);

    Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings, const std::vector<Item>& items);
}
