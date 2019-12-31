#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <array>

#include "trlevel/trtypes.h"
#include "trlevel/ILevel.h"
#include "Types.h" 

namespace trview
{
    enum class TriangulationDirection
    {
        None,
        NwSe,
        NeSw
    };

    class Sector
    {
    public:
        // Constructs sector object and parses floor data automatically 
        Sector(const trlevel::ILevel &level, const trlevel::tr3_room& room, const trlevel::tr_room_sector &sector, int sector_id, uint32_t room_number);

        // Returns the id of the room that this floor data points to 
        std::uint16_t portal() const; 

        // Gets/sets id of the sector. Used by map renderer. 
        inline int id() const { return _sector_id; }

        // Returns all neighbours for the current sector, maximum of 3 (up, down, portal). 
        std::set<std::uint16_t> neighbours() const; 

        // Returns room below 
        inline std::uint16_t room_below() const { return _room_below; }

        // Returns room above 
        inline std::uint16_t room_above() const { return _room_above; }

        // Holds "Function" enum bitwise values 
        std::uint16_t flags = 0;

        /// Get trigger information for the sector.
        const TriggerInfo& trigger() const;

        uint16_t x() const;

        uint16_t z() const;

        std::array<float, 4> corners() const;

        uint32_t room() const;

        TriangulationDirection triangulation_function() const;

        std::vector<DirectX::SimpleMath::Vector3> triangles() const;

        /// Determines whether this is a walkable floor.
        bool is_floor() const;
    private:
        bool parse(const trlevel::ILevel& level);
        void parse_slope();
        void calculate_neighbours(const trlevel::ILevel& level);

        // Holds the "wall portal" that this sector points to - this is the id of the room 
        std::uint8_t _portal, _room_above, _room_below;

        // Holds slope data 
        std::uint16_t _floor_slant{ 0 }, _ceiling_slant{ 0 };

        // Holds trigger data 
        TriggerInfo _trigger;

        // ID of the sector 
        uint16_t _sector_id; 

        // Base sector structure 
        const trlevel::tr_room_sector _sector;

        // X position of the sector in the room.
        uint16_t _x;

        // Z position of the sector in the room.
        uint16_t _z;

        // Corner heights
        std::array<float, 4> _corners;

        uint32_t _room;

        TriangulationDirection _triangulation_function{ TriangulationDirection::None };

        std::set<uint16_t> _neighbours;
    };
}