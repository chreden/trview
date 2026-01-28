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
        Sector(const trlevel::ILevel& level, const trlevel::tr3_room& room, const trlevel::tr_room_sector& sector, int sector_id, const std::weak_ptr<IRoom>& room_ptr, uint32_t sector_number);
        virtual ~Sector() = default;
        // Returns the id of the room that this floor data points to 
        virtual std::vector<std::uint16_t> portals() const override;
        // Gets/sets id of the sector. Used by map renderer. 
        virtual int id() const override { return _sector_id; }
        // Returns all neighbours for the current sector, maximum of 3 (up, down, portal). 
        virtual std::set<std::uint16_t> neighbours() const override;
        // Returns room below 
        virtual std::uint16_t room_below() const override { return _room_below; }
        // Returns room above 
        virtual std::uint16_t room_above() const override { return _room_above; }
        // Holds "Function" enum bitwise values 
        virtual SectorFlag flags() const override;
        /// Get trigger information for the sector.
        virtual TriggerInfo trigger_info() const override;
        virtual uint16_t x() const override;
        virtual uint16_t z() const override;
        virtual std::array<float, 4> corners() const override;
        virtual std::array<float, 4> ceiling_corners() const override;
        virtual DirectX::SimpleMath::Vector3 corner(Corner corner) const override;
        int8_t ceiling() const override;
        virtual DirectX::SimpleMath::Vector3 ceiling(Corner corner) const override;
        std::weak_ptr<IRoom> room() const override;
        int8_t tilt_x() const override;
        int8_t tilt_z() const override;
        virtual TriangulationDirection triangulation() const override;
        virtual std::vector<Triangle> triangles() const override;
        virtual uint32_t floordata_index() const override;
        /// Determines whether this is a walkable floor.
        virtual bool is_floor() const override;
        virtual bool is_wall() const override;
        virtual bool is_portal() const override;
        virtual bool is_ceiling() const override;
        virtual void generate_triangles() override;
        virtual void add_triangle(const ISector::Portal& portal, const Triangle& triangle, std::unordered_set<uint32_t> visited_rooms) override;
        virtual void add_flag(SectorFlag flag) override;
        void set_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        std::weak_ptr<ITrigger> trigger() const override;
        TriangulationDirection ceiling_triangulation() const override;
        uint32_t number() const override;
        int8_t floor() const override;
        uint16_t material() const override;
        uint16_t box_index() const override;
        bool stopper() const override;
        int32_t filterable_index() const override;
    private:
        bool parse(const trlevel::ILevel& level);
        void parse_slope();
        void parse_ceiling_slope();
        void calculate_neighbours(const trlevel::ILevel& level);
        void add_triangle(Triangle triangle);
        void add_quad(const ISector::Portal& portal, Quad quad);
        DirectX::SimpleMath::Vector2 corner_uv(Corner corner) const;
        void generate_floor();
        void generate_ceiling();
        void generate_ceiling_steps(const Portal& self, const Portal& north, const Portal& east, const Portal& south, const Portal& west);
        void generate_outsides(const Portal& self, const Portal& north, const Portal& east, const Portal& south, const Portal& west);

        SectorFlag _flags{ SectorFlag::None };

        // Holds the "wall portal" that this sector points to - this is the id of the room 
        std::vector<uint16_t> _portals;
        std::uint8_t _room_above;
        std::uint8_t _room_below;

        // Holds slope data 
        std::uint16_t _floor_slant{ 0 }, _ceiling_slant{ 0 };

        // Holds trigger data 
        TriggerInfo _trigger_info;
        std::weak_ptr<ITrigger> _trigger;

        // ID of the sector 
        uint16_t _sector_id; 

        // Base sector structure 
        const trlevel::tr_room_sector _sector;

        // X position of the sector in the room.
        int16_t _x;

        // Z position of the sector in the room.
        int16_t _z;

        // Corner heights
        std::array<float, 4> _corners;
        std::array<float, 4> _ceiling_corners;

        uint32_t _room;
        std::weak_ptr<IRoom> _room_ptr;

        std::optional<Triangulation> _floor_triangulation;
        TriangulationDirection _triangulation_function{ TriangulationDirection::None };
        std::optional<Triangulation> _ceiling_triangulation;
        TriangulationDirection _ceiling_triangulation_function{ TriangulationDirection::None };

        std::set<uint16_t> _neighbours;

        uint32_t _floordata_index;
        trlevel::tr_room_info _info;
        std::vector<Triangle> _triangles;
        uint32_t _number;
        int8_t _floor{ 0 };
        int8_t _ceiling{ 0 };
        uint16_t _box_index{ 0 };
        uint16_t _material{ 0 };
        bool _stopper{ false };
    };
}