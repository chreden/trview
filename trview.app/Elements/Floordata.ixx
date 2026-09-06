export module trview.app:Floordata;

import std;
import std.compat;

import trlevel;

import :Types;
import :IItem;

namespace trview
{
    export enum class FloordataMeanings
    {
        None,
        Generate
    };

    export struct Floordata
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
                MinecartRight_Mapper,
                Count
            };

            explicit Command(Function type, const std::vector<uint16_t>& data, FloordataMeanings meanings, const std::vector<std::weak_ptr<IItem>>& items, bool trng);

            Function type;
            std::vector<uint16_t> data;
            std::vector<std::string> meanings;
        private:
            void create_meanings(const std::vector<std::weak_ptr<IItem>>& items, bool trng);
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
    export Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings, bool trng, std::optional<trlevel::PlatformAndVersion> version);

    export Floordata parse_floordata(const std::vector<uint16_t>& floordata, uint32_t index, FloordataMeanings meanings, const std::vector<std::weak_ptr<IItem>>& items, bool trng, std::optional<trlevel::PlatformAndVersion> version);

    export enum class TriangulationDirection
    {
        None,
        NwSe,
        NeSw
    };

    export struct Triangulation
    {
        int16_t function;
        TriangulationDirection direction;
        float c00;
        float c01;
        float c10;
        float c11;
    };

    export Triangulation parse_triangulation(uint16_t floor, uint16_t data);

    export std::string to_string(Floordata::Command::Function function);
}
