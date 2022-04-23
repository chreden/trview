#include "Sector.h"
#include "IRoom.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    Sector::Sector(const trlevel::ILevel &level, const trlevel::tr3_room& room, const trlevel::tr_room_sector &sector, int sector_id, uint32_t room_number, const IRoom& room_ptr)
        : _sector(sector), _sector_id(static_cast<uint16_t>(sector_id)), _room_above(sector.room_above), _room_below(sector.room_below), _room(room_number), _info(room.info), _room_ptr(room_ptr)
    {
        _x = static_cast<int16_t>(sector_id / room.num_z_sectors);
        _z = static_cast<int16_t>(sector_id % room.num_z_sectors);
        parse(level);
        calculate_neighbours(level);
    }

    std::uint16_t
    Sector::portal() const
    {
        if (!(_flags & SectorFlag::Portal))
            throw std::runtime_error("Sector does not have portal function");

        return _portal;
    }

    std::set<std::uint16_t> 
    Sector::neighbours() const
    {
        return _neighbours;
    }

    bool
    Sector::parse(const trlevel::ILevel& level)
    {
        // Basic sector items 
        if (_sector.floor == -127 && _sector.ceiling == -127)
            _flags |= SectorFlag::Wall;
        if (_room_above != 0xFF)
            _flags |= SectorFlag::RoomAbove;
        if (_room_below != 0xFF)
            _flags |= SectorFlag::RoomBelow;

        // Start off the heights at the height of the floor (or in the case of a 
        // wall, at the bottom of the room).
        _corners.fill(_flags & SectorFlag::Wall ?
            level.get_room(_room).info.yBottom / trlevel::Scale_Y :
            _sector.floor * 0.25f);

        _ceiling_corners.fill(_flags & SectorFlag::Wall ?
            level.get_room(_room).info.yTop / trlevel::Scale_Y :
            _sector.ceiling * 0.25f);

        std::uint16_t cur_index = _sector.floordata_index;
        if (cur_index == 0x0)
            return true; 

        const auto max_floordata = level.num_floor_data();

        for (;;)
        {
            std::uint16_t floor = level.get_floor_data(cur_index);
            std::uint16_t subfunction = (floor & 0x7F00) >> 8; 

            switch (floor & 0x1f)
            {
            case 0x1:
                _portal = level.get_floor_data(++cur_index) & 0xFF;
                _flags |= SectorFlag::Portal;
                break; 

            case 0x2: 
            {
                _floor_slant = level.get_floor_data(++cur_index);
                _flags |= SectorFlag::FloorSlant;
                parse_slope();
                break;
            }
            case 0x3:
                _ceiling_slant = level.get_floor_data(++cur_index);
                _flags |= SectorFlag::CeilingSlant;
                parse_ceiling_slope();
                break;

            case 0x4:
            {
                std::uint16_t command = 0; 
                std::uint16_t setup = level.get_floor_data(++cur_index);

                // Basic trigger setup 
                _trigger.timer = setup & 0xFF;
                _trigger.oneshot = (setup & 0x100) >> 8; 
                _trigger.mask = (setup & 0x3E00) >> 9; 
                _trigger.sector_id = _sector_id;

                // Type of the trigger, e.g. Pad, Switch, etc.
                _trigger.type = (TriggerType) subfunction;

                bool continue_processing = true;
                if (_trigger.type == TriggerType::Key || _trigger.type == TriggerType::Switch)
                {
                    // The next element is the lock or switch - ignore.
                    auto reference = level.get_floor_data(++cur_index);
                    continue_processing = (reference & 0x8000) == 0;
                }

                // Parse actions 
                if (continue_processing)
                {
                    do
                    {
                        if (++cur_index < max_floordata)
                        {
                            command = level.get_floor_data(cur_index);
                            auto action = static_cast<TriggerCommandType>((command & 0x7C00) >> 10);
                            _trigger.commands.emplace_back(action, static_cast<uint16_t>(command & 0x3FF));
                            if (action == TriggerCommandType::Camera || action == TriggerCommandType::Flyby)
                            {
                                // Camera has another uint16_t - skip for now.
                                command = level.get_floor_data(++cur_index);
                            }
                        }

                    } while (cur_index < max_floordata && !(command & 0x8000));
                }

                _flags |= SectorFlag::Trigger;
                break; 
            }
            case 0x5: 
                _flags |= SectorFlag::Death;
                break; 

            case 0x6: // climbable walls 
                _flags |= (subfunction << 6);
                break; 

            case 0x7:
            case 0xB:
            case 0xC:
            case 0x8:
            case 0xD:
            case 0xE:
            {
                auto tri = read_triangulation(level, floor, cur_index);
                _floor_triangulation = tri;
                _triangulation_function = tri.direction;
                _corners[0] += tri.c00;
                _corners[1] += tri.c01;
                _corners[2] += tri.c10;
                _corners[3] += tri.c11;
                break;
            }
            case 0x9:
            case 0xA:
            case 0xF:
            case 0x10:
            case 0x11:
            case 0x12:
            {
                const auto tri = read_triangulation(level, floor, cur_index);
                _ceiling_triangulation = tri;
                _ceiling_triangulation_function = tri.direction;
                _ceiling_corners[3] -= tri.c00;
                _ceiling_corners[2] -= tri.c01;
                _ceiling_corners[1] -= tri.c10;
                _ceiling_corners[0] -= tri.c11;
                break;
            }
            case 0x13: 
                _flags |= SectorFlag::MonkeySwing;
                break; 
            case 0x14:
                _flags |= SectorFlag::MinecartLeft;
                break; 
            case 0x15:
                _flags |= SectorFlag::MinecartRight;
                break; 
            }

            if ((floor >> 15) || cur_index == 0x0) break; 
            else cur_index++; 
        }

        return true; 
    }

    uint16_t Sector::flags() const
    {
        return _flags;
    }

    TriggerInfo Sector::trigger() const
    {
        return _trigger;
    }

    uint16_t Sector::x() const
    {
        return static_cast<uint16_t>(_x);
    }

    uint16_t Sector::z() const
    {
        return static_cast<uint16_t>(_z);
    }

    void Sector::parse_slope()
    {
        const int8_t x_slope = _floor_slant & 0x00ff;
        const int8_t z_slope = _floor_slant >> 8;

        if (x_slope > 0)
        {
            _corners[0] += x_slope * 0.25f;
            _corners[1] += x_slope * 0.25f;
        }
        else if (x_slope < 0)
        {
            _corners[2] -= x_slope * 0.25f;
            _corners[3] -= x_slope * 0.25f;
        }

        if (z_slope > 0)
        {
            _corners[0] += z_slope * 0.25f;
            _corners[2] += z_slope * 0.25f;
        }
        else if (z_slope < 0)
        {
            _corners[1] -= z_slope * 0.25f;
            _corners[3] -= z_slope * 0.25f;
        }
    }

    void Sector::parse_ceiling_slope()
    {
        const int8_t x_slope = _ceiling_slant & 0x00ff;
        const int8_t z_slope = _ceiling_slant >> 8;

        if (x_slope > 0)
        {
            _ceiling_corners[0] -= x_slope * 0.25f;
            _ceiling_corners[1] -= x_slope * 0.25f;
        }
        else if (x_slope < 0)
        {
            _ceiling_corners[2] += x_slope * 0.25f;
            _ceiling_corners[3] += x_slope * 0.25f;
        }

        if (z_slope > 0)
        {
            _ceiling_corners[0] -= z_slope * 0.25f;
            _ceiling_corners[2] -= z_slope * 0.25f;
        }
        else if (z_slope < 0)
        {
            _ceiling_corners[1] += z_slope * 0.25f;
            _ceiling_corners[3] += z_slope * 0.25f;
        }
    }

    std::array<float, 4> Sector::corners() const
    {
        return _corners;
    }

    std::array<float, 4> Sector::ceiling_corners() const
    {
        return _ceiling_corners;
    }

    Vector3 Sector::corner(Corner corner) const
    {
        const float x = _x + 0.5f;
        const float z = _z + 0.5f;
        switch (corner)
        {
            case Corner::NW:
                return Vector3(x - 0.5f, _corners[1], z + 0.5f);
            case Corner::NE:
                return Vector3(x + 0.5f, _corners[3], z + 0.5f);
            case Corner::SE:
                return Vector3(x + 0.5f, _corners[2], z - 0.5f);
            case Corner::SW:
                return Vector3(x - 0.5f, _corners[0], z - 0.5f);
        }
        return Vector3::Zero;
    }

    Vector3 Sector::ceiling(Corner corner) const
    {
        const float x = _x + 0.5f;
        const float z = _z + 0.5f;
        switch (corner)
        {
            case Corner::NW:
                return Vector3(x - 0.5f, _ceiling_corners[3], z + 0.5f);
            case Corner::NE:
                return Vector3(x + 0.5f, _ceiling_corners[1], z + 0.5f);
            case Corner::SE:
                return Vector3(x + 0.5f, _ceiling_corners[0], z - 0.5f);
            case Corner::SW:
                return Vector3(x - 0.5f, _ceiling_corners[2], z - 0.5f);
        }
        return Vector3::Zero;
    }

    uint32_t Sector::room() const
    {
        return _room;
    }

    ISector::TriangulationDirection Sector::triangulation_function() const
    {
        return _triangulation_function;
    }

    std::vector<ISector::Triangle> Sector::triangles() const
    {
        return _triangles;
    }

    bool Sector::is_floor() const
    {
        return room_below() == 0xff && !(_flags & SectorFlag::Wall) && !(_flags & SectorFlag::Portal);
    }

    void Sector::calculate_neighbours(const trlevel::ILevel& level)
    {
        const auto add_neighbour = [&](std::uint16_t room)
        {
            const auto &r = level.get_room(room);
            if (r.alternate_room != -1)
            {
                _neighbours.insert(r.alternate_room);
            }

            _neighbours.insert(room);
        };

        if (_flags & SectorFlag::Portal)
        {
            add_neighbour(_portal);
        }

        if (_flags & SectorFlag::RoomAbove)
        {
            add_neighbour(_room_above);
        }

        if (_flags & SectorFlag::RoomBelow)
        {
            add_neighbour(_room_below);
        }
    }

    bool Sector::is_wall() const
    {
        return _flags & SectorFlag::Wall;
    }

    bool Sector::is_portal() const
    {
        return _flags & SectorFlag::Portal;
    }

    bool Sector::is_ceiling() const
    {
        return _room_above == 0xff && !is_wall() && !is_portal();
    }

    void Sector::generate_triangles()
    {
        auto& tris = _triangles;

        const auto self = _room_ptr.sector_portal(_x, _z, _x, _z);
        const auto north = _room_ptr.sector_portal(_x, _z, _x, _z + 1);
        const auto south = _room_ptr.sector_portal(_x, _z, _x, _z - 1);
        const auto east = _room_ptr.sector_portal(_x, _z, _x + 1, _z);
        const auto west = _room_ptr.sector_portal(_x, _z, _x - 1, _z);

        if (_ceiling_triangulation.has_value())
        {
            const auto function = _ceiling_triangulation.value().function;
            if (_ceiling_triangulation_function == TriangulationDirection::NwSe)
            {
                if (function != 0x0F)
                {
                    tris.push_back(Triangle(ceiling(Corner::NW), ceiling(Corner::SW), ceiling(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::SW), corner_uv(Corner::SE), Triangle::Type::Floor, _room));
                }
                if (function != 0x10)
                {
                    tris.push_back(Triangle(ceiling(Corner::SE), ceiling(Corner::NE), ceiling(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::NE), corner_uv(Corner::NW), Triangle::Type::Floor, _room));
                }
            }
            else
            {
                if (function != 0x11)
                {
                    tris.push_back(Triangle(ceiling(Corner::NW), ceiling(Corner::SW), ceiling(Corner::NE), corner_uv(Corner::NW), corner_uv(Corner::SW), corner_uv(Corner::NE), Triangle::Type::Floor, _room));
                }
                if (function != 0x12)
                {
                    tris.push_back(Triangle(ceiling(Corner::SW), ceiling(Corner::SE), ceiling(Corner::NE), corner_uv(Corner::SW), corner_uv(Corner::SE), corner_uv(Corner::NE), Triangle::Type::Floor, _room));
                }
            }
        }
        else if (is_ceiling())
        {
            add_quad(self, Quad(ceiling(Corner::SW), ceiling(Corner::NE), ceiling(Corner::NW), ceiling(Corner::SE), Triangle::Type::Floor, _room));

            if (north && !north.is_wall() && (ceiling(Corner::NW) != north.ceiling(Corner::SW) || ceiling(Corner::NE) != north.ceiling(Corner::SE)))
            {
                add_quad(self, Quad(ceiling(Corner::NW), north.ceiling(Corner::SE), north.ceiling(Corner::SW), ceiling(Corner::NE), Triangle::Type::Floor, _room));
            }

            if (south && !south.is_wall() && (ceiling(Corner::SW) != south.ceiling(Corner::NW) || ceiling(Corner::SE) != south.ceiling(Corner::NE)))
            {
                add_quad(self, Quad(south.ceiling(Corner::NW), ceiling(Corner::SE), ceiling(Corner::SW), south.ceiling(Corner::NE), Triangle::Type::Floor, _room));
            }

            if (east && !east.is_wall() && (ceiling(Corner::NE) != east.ceiling(Corner::NW) || ceiling(Corner::SE) != east.ceiling(Corner::SW)))
            {
                add_quad(self, Quad(east.ceiling(Corner::SW), ceiling(Corner::NE), ceiling(Corner::SE), east.ceiling(Corner::NW), Triangle::Type::Floor, _room));
            }

            if (west && !west.is_wall() && (ceiling(Corner::NW) != west.ceiling(Corner::NE) || ceiling(Corner::SW) != west.ceiling(Corner::SE)))
            {
                add_quad(self, Quad(ceiling(Corner::SW), west.ceiling(Corner::NE), west.ceiling(Corner::SE), ceiling(Corner::NW), Triangle::Type::Floor, _room));
            }
        }

        if (_floor_triangulation.has_value())
        {
            const auto function = _floor_triangulation.value().function;
            if (_triangulation_function == TriangulationDirection::NwSe)
            {
                if (function != 0x0B)
                {
                    tris.push_back(Triangle(corner(Corner::SE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::SW), corner_uv(Corner::NW), Triangle::Type::Floor, _room));
                }
                if (function != 0x0C)
                {
                    tris.push_back(Triangle(corner(Corner::NW), corner(Corner::NE), corner(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SE), Triangle::Type::Floor, _room));
                }
            }
            else
            {
                if (function != 0x0D)
                {
                    tris.push_back(Triangle(corner(Corner::NE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SW), corner_uv(Corner::NW), Triangle::Type::Floor, _room));
                }
                if (function != 0x0E)
                {
                    tris.push_back(Triangle(corner(Corner::NE), corner(Corner::SE), corner(Corner::SW), corner_uv(Corner::NE), corner_uv(Corner::SE), corner_uv(Corner::SW), Triangle::Type::Floor, _room));
                }
            }
        }
        else if (is_floor())
        {
            if (_triangulation_function == TriangulationDirection::NwSe)
            {
                tris.push_back(Triangle( corner(Corner::SE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::SW), corner_uv(Corner::NW), Triangle::Type::Floor, _room ));
                tris.push_back(Triangle( corner(Corner::NW), corner(Corner::NE), corner(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SE), Triangle::Type::Floor, _room ));
            }
            else
            {
                tris.push_back(Triangle( corner(Corner::NE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SW), corner_uv(Corner::NW), Triangle::Type::Floor, _room));
                tris.push_back(Triangle( corner(Corner::NE), corner(Corner::SE), corner(Corner::SW), corner_uv(Corner::NE), corner_uv(Corner::SE), corner_uv(Corner::SW), Triangle::Type::Floor, _room ));
            }
        } 

        if (is_wall())
        {
            if (north && !north.is_wall() && !north.is_portal())
            {
                add_quad(self, Quad(north.ceiling(Corner::SE), north.corner(Corner::SW), north.corner(Corner::SE), north.ceiling(Corner::SW), Triangle::Type::Wall, _room));
            }

            if (south && !south.is_wall() && !south.is_portal())
            {
                add_quad(self, Quad(south.ceiling(Corner::NW), south.corner(Corner::NE), south.corner(Corner::NW), south.ceiling(Corner::NE), Triangle::Type::Wall, _room));
            }

            if (east && !east.is_wall() && !east.is_portal())
            {
                add_quad(self, Quad(east.ceiling(Corner::SW), east.corner(Corner::NW), east.corner(Corner::SW), east.ceiling(Corner::NW), Triangle::Type::Wall, _room));
            }

            if (west && !west.is_wall() && !west.is_portal())
            {
                add_quad(self, Quad(west.ceiling(Corner::NE), west.corner(Corner::SE), west.corner(Corner::NE), west.ceiling(Corner::SE), Triangle::Type::Wall, _room));
            }
        }
        else if (!is_portal())
        {
            if (north && !(north.flags() & SectorFlag::Wall))
            {
                if (corner(Corner::NW) != north.corner(Corner::SW) || corner(Corner::NE) != north.corner(Corner::SE))
                {
                    add_quad(self, Quad(north.corner(Corner::SW), corner(Corner::NE), corner(Corner::NW), north.corner(Corner::SE), Triangle::Type::Floor, _room));
                }
            }

            if (south && !(south.flags() & SectorFlag::Wall))
            {
                if (corner(Corner::SW) != south.corner(Corner::NW) || corner(Corner::SE) != south.corner(Corner::NE))
                {
                    add_quad(self, Quad(corner(Corner::SW), south.corner(Corner::NE), south.corner(Corner::NW), corner(Corner::SE), Triangle::Type::Floor, _room));
                }
            }

            if (east && !(east.flags() & SectorFlag::Wall))
            {
                if (corner(Corner::NE) != east.corner(Corner::NW) || corner(Corner::SE) != east.corner(Corner::SW))
                {
                    add_quad(self, Quad(corner(Corner::SE), east.corner(Corner::NW), east.corner(Corner::SW), corner(Corner::NE), Triangle::Type::Floor, _room));
                }
            }

            if (west && !(west.flags() & SectorFlag::Wall))
            {
                if (corner(Corner::NW) != west.corner(Corner::NE) || corner(Corner::SW) != west.corner(Corner::SE))
                {
                    add_quad(self, Quad(west.corner(Corner::SE), corner(Corner::NW), corner(Corner::SW), west.corner(Corner::NE), Triangle::Type::Floor, _room));
                }
            }
        }
    }

    void Sector::add_triangle(const ISector::Portal& portal, const Triangle& triangle)
    {
        // If the triangle goes above the top of the room and there is a room above then
        // split the triangles and send them to the above room. The remainder is added to
        // this room.
        if (triangle.v0.y < portal.direct_room->y_top() ||
            triangle.v1.y < portal.direct_room->y_top() ||
            triangle.v2.y < portal.direct_room->y_top())
        {
            // Limit the triangle to the height of the room
            Triangle tri = triangle;
            tri.v0.y = std::max(tri.v0.y, portal.direct_room->y_top());
            tri.v1.y = std::max(tri.v1.y, portal.direct_room->y_top());
            tri.v2.y = std::max(tri.v2.y, portal.direct_room->y_top());
            add_triangle(tri);

            if (portal.sector_above)
            {
                Triangle offcut = triangle;
                offcut.v0.y = std::min(offcut.v0.y, portal.direct_room->y_top());
                offcut.v1.y = std::min(offcut.v1.y, portal.direct_room->y_top());
                offcut.v2.y = std::min(offcut.v2.y, portal.direct_room->y_top());

                offcut.v0 -= portal.above_offset;
                offcut.v1 -= portal.above_offset;
                offcut.v2 -= portal.above_offset;

                auto above_portal = portal.room_above->sector_portal(portal.sector_above->x(), portal.sector_above->z(),
                    portal.sector_above->x(), portal.sector_above->z());
                portal.sector_above->add_triangle(above_portal, offcut);
            }
        }
        else
        {
            add_triangle(triangle);
        }
    }

    void Sector::add_triangle(Triangle triangle)
    {
        // Check if the triangle exists already - don't add it if it does.
        for (const auto& existing : _triangles)
        {
            if (existing == triangle)
            {
                return;
            }
        }

        // TODO: Subdivide the triangle into 1x1 block heights.
        triangle.room = _room;
        _triangles.push_back(triangle);
    }

    void Sector::add_quad(const ISector::Portal& portal, Quad quad)
    {
        // Adding a quad will just add a series of triangles.
        auto triangles = quad.triangles();
        for (const auto& triangle : triangles)
        {
            add_triangle(triangle);
        }
    }

    Vector2 Sector::corner_uv(Corner corner) const
    {
        switch (corner)
        {
        case Corner::SE:
            return Vector2(1, 0);
        case Corner::NE:
            return Vector2(1, 1);
        case Corner::NW:
            return Vector2(0, 1);
        case Corner::SW:
            return Vector2(0, 0);
        }
        return Vector2::Zero;
    }

    Triangulation read_triangulation(const trlevel::ILevel& level, uint16_t floor, std::uint16_t& cur_index)
    {
        // Not sure what to do with h1 and h2 values yet.
        const int16_t h2 = (floor & 0x7C00) >> 10;
        const int16_t h1 = (floor & 0x03E0) >> 5;
        const int16_t function = (floor & 0x001F);

        ISector::TriangulationDirection direction{ ISector::TriangulationDirection::None };
        switch (function)
        {
        case 0x07:
        case 0x0B:
        case 0x0C:
            direction = ISector::TriangulationDirection::NwSe;
            break;
        case 0x08:
        case 0x0D:
        case 0x0E:
            direction = ISector::TriangulationDirection::NeSw;
            break;
        }

        const uint16_t corner_values = level.get_floor_data(++cur_index);
        const uint16_t c00 = (corner_values & 0x00F0) >> 4;
        const uint16_t c01 = (corner_values & 0x0F00) >> 8;
        const uint16_t c10 = (corner_values & 0x000F);
        const uint16_t c11 = (corner_values & 0xF000) >> 12;
        const auto max_corner = std::max({ c00, c01, c10, c11 });
        return Triangulation{ function, direction, (max_corner - c00) * 0.25f, (max_corner - c01) * 0.25f, (max_corner - c10) * 0.25f, (max_corner - c11) * 0.25f };
    }
}

