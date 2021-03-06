#pragma once

#include <memory>
#include <utility>
#include "ISector.h"

namespace trview
{
    class Sector final : public ISector
    {
    public:
        // Constructs sector object and parses floor data automatically 
        Sector(const trlevel::ILevel &level, const trlevel::tr3_room& room, const trlevel::tr_room_sector &sector, int sector_id, uint32_t room_number);

        virtual ~Sector() = default;

        // Returns the id of the room that this floor data points to 
        virtual std::uint16_t portal() const override;

        // Gets/sets id of the sector. Used by map renderer. 
        virtual int id() const override { return _sector_id; }

        // Returns all neighbours for the current sector, maximum of 3 (up, down, portal). 
        virtual std::set<std::uint16_t> neighbours() const override;

        // Returns room below 
        virtual std::uint16_t room_below() const override { return _room_below; }

        // Returns room above 
        virtual std::uint16_t room_above() const override { return _room_above; }

        // Holds "Function" enum bitwise values 
        virtual uint16_t flags() const override;

        /// Get trigger information for the sector.
        virtual TriggerInfo trigger() const override;

        virtual uint16_t x() const override;

        virtual uint16_t z() const override;

        virtual std::array<float, 4> corners() const override;

        virtual uint32_t room() const override;

        virtual TriangulationDirection triangulation_function() const override;

        virtual std::vector<DirectX::SimpleMath::Vector3> triangles() const override;

        /// Determines whether this is a walkable floor.
        virtual bool is_floor() const override;
    private:
        bool parse(const trlevel::ILevel& level);
        void parse_slope();
        void calculate_neighbours(const trlevel::ILevel& level);

        uint16_t _flags{ 0u };

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