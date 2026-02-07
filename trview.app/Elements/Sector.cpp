#include "Sector.h"
#include "IRoom.h"
#include "Floordata.h"
#include <trview.common/Algorithms.h>
#include <ranges>
#include "ILevel.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        void add_quad_remote(const ISector::Portal& self, const ISector::Portal& target, ISector::Quad quad, SectorFlag remote_filter, const Vector3& away)
        {
            if (target.is_portal() && target.target)
            {
                const auto target_room = target.target->room().lock();
                const auto source_room_info = self.direct_room->info();
                const auto target_room_info = target_room->info();
                // Only add remote if the target ceiling is higher than ours
                if (target_room_info.yTop < source_room_info.yTop)
                {
                    const auto target_portal = target_room->sector_portal(target.target->x(), target.target->z(), target.target->x(), target.target->z());
                    quad.room = target_room->number();
                    quad.type = (target.target->flags() & ~(SectorFlag::Death | SectorFlag::MonkeySwing)) & remote_filter;
                    for (auto& v : quad.v)
                    {
                        v -= target.offset;
                    }
                    for (const auto& triangle : quad.triangles())
                    {
                        target.target->add_triangle(target_portal, triangle, {});
                    }

                    // If the remote is an alternate or has an alternate also add geometry to that room.
                    if (target_room->alternate_mode() != IRoom::AlternateMode::None)
                    {
                        if (auto level = target_room->level().lock())
                        {
                            if (auto alternate_room = level->room(target_room->alternate_room()).lock())
                            {
                                const auto alternate_target_portal = alternate_room->sector_portal(target.target->x(), target.target->z(), target.target->x(), target.target->z());
                                quad.room = alternate_room->number();
                                const auto flag = alternate_target_portal.target ? alternate_target_portal.target->flags() : alternate_target_portal.direct->flags();
                                quad.type = (flag & ~(SectorFlag::Death | SectorFlag::MonkeySwing)) & remote_filter;
                                for (const auto& triangle : quad.triangles())
                                {
                                    alternate_target_portal.direct->add_triangle(alternate_target_portal, triangle, {});
                                }
                            }
                        }
                    }

                    return;
                }
            }

            auto triangles = quad.triangles();
            if (triangles[0].normal == away)
            {
                for (auto& triangle : triangles)
                {
                    const auto flag = target.target ? target.target->flags() : target.direct->flags();
                    triangle.type = (flag & ~(SectorFlag::Death | SectorFlag::MonkeySwing)) & remote_filter;
                    target.direct->add_triangle(self, triangle, {});
                }
            }
            else
            {
                for (auto& triangle : triangles)
                {
                    self.direct->add_triangle(self, triangle, {});
                }
            }
        }
    }

    Sector::Sector(const trlevel::ILevel& level, const trlevel::tr3_room& room, const trlevel::tr_room_sector& sector, int sector_id, const std::weak_ptr<IRoom>& room_ptr, uint32_t sector_number)
        : _sector(sector), _sector_id(static_cast<uint16_t>(sector_id)), _room_above(sector.room_above), _room_below(sector.room_below), _room(room_number(room_ptr)), _info(room.info), _room_ptr(room_ptr),
        _floordata_index(sector.floordata_index), _number(sector_number), _floor(sector.floor), _ceiling(sector.ceiling), _box_index(sector.box_index)
    {
        _x = static_cast<int16_t>(sector_id / room.num_z_sectors);
        _z = static_cast<int16_t>(sector_id % room.num_z_sectors);
        parse(level);
        calculate_neighbours(level);

        if (level.platform_and_version().version >= trlevel::LevelVersion::Tomb3)
        {
            _material = _box_index & 0x000F;
            _stopper = (_box_index & 0x8000) != 0;
            _box_index = (_box_index & 0x7FF0) >> 4;
        }
    }

    std::vector<std::uint16_t> Sector::portals() const
    {
        return _portals;
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
        {
            _flags |= SectorFlag::Wall;
            const auto info = level.get_room(_room);
            if ((_x > 0 && _z > 0) && (_x < info.num_x_sectors - 1 && _z < info.num_z_sectors - 1))
            {
                _flags |= SectorFlag::SpecialWall;
            }
        }
        if (_room_above != 0xFF)
            _flags |= SectorFlag::RoomAbove;
        if (_room_below != 0xFF)
            _flags |= SectorFlag::RoomBelow;

        // Start off the heights at the height of the floor (or in the case of a 
        // wall, at the bottom of the room).
        _corners.fill(has_flag(_flags, SectorFlag::Wall) ?
            level.get_room(_room).info.yBottom / trlevel::Scale_Y :
            _sector.floor * 0.25f);

        _ceiling_corners.fill(has_flag(_flags, SectorFlag::Wall) ?
            level.get_room(_room).info.yTop / trlevel::Scale_Y :
            _sector.ceiling * 0.25f);

        if (_sector.floordata_index != 0)
        {
            auto floordata = parse_floordata(level.get_floor_data_all(), _sector.floordata_index, FloordataMeanings::None, level.trng(), level.platform_and_version());

            for (const auto& command : floordata.commands)
            {
                using Function = Floordata::Command::Function;
                const uint16_t floor = command.data[0];
                std::uint16_t subfunction = (floor & 0x7F00) >> 8;

                switch (command.type)
                {
                case Function::Portal:
                {
                    _flags |= SectorFlag::Portal;
                    _portals.push_back(command.data[1]);
                    break;
                }
                case Function::FloorSlant:
                {
                    _floor_slant = command.data[1];
                    _flags |= SectorFlag::FloorSlant;
                    parse_slope();
                    break;
                }
                case Function::CeilingSlant:
                {
                    _ceiling_slant = command.data[1];
                    _flags |= SectorFlag::CeilingSlant;
                    parse_ceiling_slope();
                    break;
                }
                case Function::Trigger:
                {
                    _flags |= SectorFlag::Trigger;

                    uint32_t index = 0;

                    // Basic trigger setup 
                    const std::uint16_t setup = command.data[++index];
                    _trigger_info.timer = setup & 0xFF;
                    _trigger_info.oneshot = (setup & 0x100) >> 8;
                    _trigger_info.mask = (setup & 0x3E00) >> 9;
                    _trigger_info.sector_id = _sector_id;

                    // Type of the trigger, e.g. Pad, Switch, etc.
                    _trigger_info.type = static_cast<TriggerType>(subfunction);

                    bool continue_processing = true;
                    if (_trigger_info.type == TriggerType::Key || _trigger_info.type == TriggerType::Switch)
                    {
                        // The next element is the lock or switch - ignore.
                        auto reference = command.data[++index];
                        continue_processing = (reference & 0x8000) == 0;
                    }

                    uint16_t trigger_command = 0;

                    // Parse actions 
                    if (continue_processing)
                    {
                        do
                        {
                            if (++index < command.data.size())
                            {
                                trigger_command = command.data[index];
                                auto action = static_cast<TriggerCommandType>((trigger_command & 0x7C00) >> 10);
                                if (action == TriggerCommandType::Camera || 
                                    action == TriggerCommandType::Flyby || 
                                    (level.trng() && action == TriggerCommandType::Flipeffect))
                                {
                                    uint16_t next_trigger_command = command.data[++index];
                                    _trigger_info.commands.push_back({ .type = action, .data = 
                                        {
                                            static_cast<uint16_t>(trigger_command & 0x3ff),
                                            static_cast<uint16_t>(next_trigger_command & 0x7ff)
                                        }});
                                    trigger_command = next_trigger_command;
                                }
                                else
                                {
                                    _trigger_info.commands.push_back({ .type = action, .data = { static_cast<uint16_t>(trigger_command & 0x3ff) }});
                                }
                            }

                        } while (index < command.data.size() && !(trigger_command & 0x8000));
                    }

                    break;
                }
                case Function::Death:
                {
                    _flags |= SectorFlag::Death;
                    break;
                }
                case Function::ClimbableWall:
                {
                    _flags |= static_cast<SectorFlag>(subfunction << 6);
                    break;
                }
                case Function::Triangulation_Floor_NWSE:
                case Function::Triangulation_Floor_NESW:
                case Function::Triangulation_Floor_Collision_SW:
                case Function::Triangulation_Floor_Collision_NE:
                case Function::Triangulation_Floor_Collision_SE:
                case Function::Triangulation_Floor_Collision_NW:
                {
                    auto tri = parse_triangulation(floor, command.data[1]);
                    _floor_triangulation = tri;
                    _triangulation_function = tri.direction;
                    _corners[0] += tri.c00;
                    _corners[1] += tri.c01;
                    _corners[2] += tri.c10;
                    _corners[3] += tri.c11;
                    break;
                }
                case Function::Triangulation_Ceiling_NW:
                case Function::Triangulation_Ceiling_NE:
                case Function::Triangulation_Ceiling_Collision_SW:
                case Function::Triangulation_Ceiling_Collision_NE:
                case Function::Triangulation_Ceiling_Collision_NW:
                case Function::Triangulation_Ceiling_Collision_SE:
                {
                    auto tri = parse_triangulation(floor, command.data[1]);
                    _ceiling_triangulation = tri;
                    _ceiling_triangulation_function = tri.direction;
                    _ceiling_corners[3] -= tri.c00;
                    _ceiling_corners[2] -= tri.c01;
                    _ceiling_corners[1] -= tri.c10;
                    _ceiling_corners[0] -= tri.c11;
                    break;
                }
                case Function::MonkeySwing:
                {
                    _flags |= SectorFlag::MonkeySwing;
                    break;
                }
                case Function::MinecartLeft_DeferredTrigger:
                {
                    _flags |= SectorFlag::MinecartLeft;
                    break;
                }
                case Function::MinecartRight_Mapper:
                {
                    _flags |= SectorFlag::MinecartRight;
                    break;
                }
                }
            }
        }

        return true; 
    }

    SectorFlag Sector::flags() const
    {
        return _flags;
    }

    TriggerInfo Sector::trigger_info() const
    {
        return _trigger_info;
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
        const int8_t x_slope = tilt_x();
        const int8_t z_slope = tilt_z();

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

    std::weak_ptr<IRoom> Sector::room() const
    {
        return _room_ptr;
    }

    int8_t Sector::tilt_x() const
    {
        return static_cast<int8_t>(_floor_slant & 0x00ff);
    }

    int8_t Sector::tilt_z() const
    {
        return static_cast<int8_t>(_floor_slant >> 8);
    }

    TriangulationDirection Sector::triangulation() const
    {
        return _triangulation_function;
    }

    std::vector<ISector::Triangle> Sector::triangles() const
    {
        return _triangles;
    }

    bool Sector::is_floor() const
    {
        return room_below() == 0xff && !has_any_flag(_flags, SectorFlag::Wall, SectorFlag::Portal);
    }

    void Sector::calculate_neighbours(const trlevel::ILevel& level)
    {
        const auto add_neighbour = [&](std::uint16_t room)
        {
            if (room == 0xff || room >= level.num_rooms())
            {
                return;
            }

            const auto &r = level.get_room(room);
            if (r.alternate_room != -1)
            {
                _neighbours.insert(r.alternate_room);
            }

            _neighbours.insert(room);
        };

        if (has_flag(_flags, SectorFlag::Portal))
        {
            for (const auto& portal : _portals)
            {
                add_neighbour(portal);
            }
        }

        if (has_flag(_flags, SectorFlag::RoomAbove))
        {
            add_neighbour(_room_above);
        }

        if (has_flag(_flags, SectorFlag::RoomBelow))
        {
            add_neighbour(_room_below);
        }
    }

    uint32_t Sector::floordata_index() const
    {
        return _floordata_index;
    }

    bool Sector::is_wall() const
    {
        return has_flag(_flags, SectorFlag::Wall);
    }

    bool Sector::is_portal() const
    {
        return has_flag(_flags, SectorFlag::Portal);
    }

    bool Sector::is_ceiling() const
    {
        return _room_above == 0xff && !is_wall() && !is_portal();
    }

    void Sector::generate_triangles()
    {
        auto room = _room_ptr.lock();
        if (!room)
        {
            return;
        }

        const auto level = room->level().lock();
        if (!level)
        {
            return;
        }

        const auto self = room->sector_portal(_x, _z, _x, _z);
        const auto north = room->sector_portal(_x, _z, _x, _z + 1);
        const auto south = room->sector_portal(_x, _z, _x, _z - 1);
        const auto east = room->sector_portal(_x, _z, _x + 1, _z);
        const auto west = room->sector_portal(_x, _z, _x - 1, _z);

        if (self.is_portal())
        {
            return;
        }

        generate_floor();
        generate_ceiling();
        generate_ceiling_steps(self, north, east, south, west);
        generate_outsides(self, north, east, south, west);
    }

    void Sector::add_triangle(const ISector::Portal& portal, const Triangle& triangle, std::unordered_set<uint32_t> visited_rooms)
    {
        visited_rooms.insert(_room);

        // If the triangle goes above the top of the room and there is a room above then
        // split the triangles and send them to the above room. The remainder is added to
        // this room.
        if (triangle.v0.y < portal.direct_room->y_top() ||
            triangle.v1.y < portal.direct_room->y_top() ||
            triangle.v2.y < portal.direct_room->y_top())
        {
            // Limit the triangle to the height of the room
            Triangle tri = triangle;

            tri.uv0.y = tri.v0.y = std::max(tri.v0.y, portal.direct_room->y_top());
            tri.uv1.y = tri.v1.y = std::max(tri.v1.y, portal.direct_room->y_top());
            tri.uv2.y = tri.v2.y = std::max(tri.v2.y, portal.direct_room->y_top());

            add_triangle(tri);

            if (portal.sector_above)
            {
                auto above = portal.sector_above->room().lock();
                if (visited_rooms.find(above->number()) == visited_rooms.end())
                {
                    Triangle offcut = triangle;
                    offcut.uv0.y = offcut.v0.y = std::min(offcut.v0.y, portal.direct_room->y_top());
                    offcut.uv1.y = offcut.v1.y = std::min(offcut.v1.y, portal.direct_room->y_top());
                    offcut.uv2.y = offcut.v2.y = std::min(offcut.v2.y, portal.direct_room->y_top());

                    offcut.v0 -= portal.above_offset;
                    offcut.v1 -= portal.above_offset;
                    offcut.v2 -= portal.above_offset;

                    auto above_portal = portal.room_above->sector_portal(portal.sector_above->x(), portal.sector_above->z(),
                        portal.sector_above->x(), portal.sector_above->z());
                    portal.sector_above->add_triangle(above_portal, offcut, visited_rooms);
                }
            }
        }
        else if (triangle.v0.y > portal.direct_room->y_bottom() ||
                 triangle.v1.y > portal.direct_room->y_bottom() ||
                 triangle.v2.y > portal.direct_room->y_bottom())
        {
            // Limit the triangle to the height of the room
            Triangle tri = triangle;

            tri.uv0.y = tri.v0.y = std::min(tri.v0.y, portal.direct_room->y_bottom());
            tri.uv1.y = tri.v1.y = std::min(tri.v1.y, portal.direct_room->y_bottom());
            tri.uv2.y = tri.v2.y = std::min(tri.v2.y, portal.direct_room->y_bottom());

            add_triangle(tri);
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

        triangle.room = _room;
        _triangles.push_back(triangle);
    }

    void Sector::add_quad(const ISector::Portal& portal, Quad quad)
    {
        // Adding a quad will just add a series of triangles.
        auto triangles = quad.triangles();
        for (const auto& triangle : triangles)
        {
            add_triangle(portal, triangle, {});
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

    void Sector::add_flag(SectorFlag flag)
    {
        _flags |= flag;
    }

    void Sector::set_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _trigger = trigger;
    }

    std::weak_ptr<ITrigger> Sector::trigger() const
    {
        return _trigger;
    }

    TriangulationDirection Sector::ceiling_triangulation() const
    {
        return _ceiling_triangulation_function;
    }

    uint32_t Sector::number() const
    {
        return _number;
    }

    void Sector::generate_floor()
    {
        if (is_wall())
        {
            return;
        }

        const SectorFlag floor_flags = _flags & ~(SectorFlag::MonkeySwing | SectorFlag::Climbable);
        auto& tris = _triangles;
        if (_floor_triangulation.has_value())
        {
            const auto function = _floor_triangulation.value().function;
            if (_triangulation_function == TriangulationDirection::NwSe)
            {
                if (function != 0x0B)
                {
                    tris.push_back(Triangle(corner(Corner::SE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::SW), corner_uv(Corner::NW), floor_flags, _room));
                }
                if (function != 0x0C)
                {
                    tris.push_back(Triangle(corner(Corner::NW), corner(Corner::NE), corner(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SE), floor_flags, _room));
                }
            }
            else
            {
                if (function != 0x0D)
                {
                    tris.push_back(Triangle(corner(Corner::NE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SW), corner_uv(Corner::NW), floor_flags, _room));
                }
                if (function != 0x0E)
                {
                    tris.push_back(Triangle(corner(Corner::NE), corner(Corner::SE), corner(Corner::SW), corner_uv(Corner::NE), corner_uv(Corner::SE), corner_uv(Corner::SW), floor_flags, _room));
                }
            }
        }
        else if (is_floor())
        {
            if (_triangulation_function == TriangulationDirection::NwSe)
            {
                tris.push_back(Triangle(corner(Corner::SE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::SW), corner_uv(Corner::NW), floor_flags, _room));
                tris.push_back(Triangle(corner(Corner::NW), corner(Corner::NE), corner(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SE), floor_flags, _room));
            }
            else
            {
                tris.push_back(Triangle(corner(Corner::NE), corner(Corner::SW), corner(Corner::NW), corner_uv(Corner::NE), corner_uv(Corner::SW), corner_uv(Corner::NW), floor_flags, _room));
                tris.push_back(Triangle(corner(Corner::NE), corner(Corner::SE), corner(Corner::SW), corner_uv(Corner::NE), corner_uv(Corner::SE), corner_uv(Corner::SW), floor_flags, _room));
            }
        }
    }

    void Sector::generate_ceiling()
    {
        if (is_wall())
        {
            return;
        }

        auto& tris = _triangles;
        const SectorFlag ceiling_flags = _flags & ~(SectorFlag::Death | SectorFlag::Climbable);
        if (_ceiling_triangulation.has_value())
        {
            const auto function = _ceiling_triangulation.value().function;
            if (_ceiling_triangulation_function == TriangulationDirection::NwSe)
            {
                if (function != 0x0F)
                {
                    tris.push_back(Triangle(ceiling(Corner::NW), ceiling(Corner::SW), ceiling(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::SW), corner_uv(Corner::SE), ceiling_flags, _room));
                }
                if (function != 0x10)
                {
                    tris.push_back(Triangle(ceiling(Corner::SE), ceiling(Corner::NE), ceiling(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::NE), corner_uv(Corner::NW), ceiling_flags, _room));
                }
            }
            else
            {
                if (function != 0x11)
                {
                    tris.push_back(Triangle(ceiling(Corner::NW), ceiling(Corner::SW), ceiling(Corner::NE), corner_uv(Corner::NW), corner_uv(Corner::SW), corner_uv(Corner::NE), ceiling_flags, _room));
                }
                if (function != 0x12)
                {
                    tris.push_back(Triangle(ceiling(Corner::SW), ceiling(Corner::SE), ceiling(Corner::NE), corner_uv(Corner::SW), corner_uv(Corner::SE), corner_uv(Corner::NE), ceiling_flags, _room));
                }
            }
        }
        else if (is_ceiling())
        {
            tris.push_back(Triangle(ceiling(Corner::NW), ceiling(Corner::SW), ceiling(Corner::SE), corner_uv(Corner::NW), corner_uv(Corner::SW), corner_uv(Corner::SE), ceiling_flags, _room));
            tris.push_back(Triangle(ceiling(Corner::SE), ceiling(Corner::NE), ceiling(Corner::NW), corner_uv(Corner::SE), corner_uv(Corner::NE), corner_uv(Corner::NW), ceiling_flags, _room));
        }
    }

    void Sector::generate_ceiling_steps(const Portal& self, const Portal& north, const Portal& east, const Portal& south, const Portal& west)
    {
        if (!is_ceiling())
        {
            return;
        }

        const SectorFlag wall_flags = _flags & ~(SectorFlag::Death | SectorFlag::MonkeySwing);
        if (north && (north.is_portal() || !north.is_wall()) && (ceiling(Corner::NW) != north.ceiling(Corner::SW) || ceiling(Corner::NE) != north.ceiling(Corner::SE)))
        {
            add_quad_remote(self, north, Quad{ ceiling(Corner::NW), north.ceiling(Corner::SE), north.ceiling(Corner::SW), ceiling(Corner::NE), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableNorth), _room }, (~SectorFlag::Climbable | SectorFlag::ClimbableSouth), Vector3::Backward);
        }

        if (south && (south.is_portal() || !south.is_wall()) && (ceiling(Corner::SW) != south.ceiling(Corner::NW) || ceiling(Corner::SE) != south.ceiling(Corner::NE)))
        {
            add_quad_remote(self, south, Quad{ south.ceiling(Corner::NW), ceiling(Corner::SE), ceiling(Corner::SW), south.ceiling(Corner::NE), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableSouth), _room }, (~SectorFlag::Climbable | SectorFlag::ClimbableNorth), Vector3::Forward);
        }

        if (east && (east.is_portal() || !east.is_wall()) && (ceiling(Corner::NE) != east.ceiling(Corner::NW) || ceiling(Corner::SE) != east.ceiling(Corner::SW)))
        {
            add_quad_remote(self, east, Quad{ east.ceiling(Corner::SW), ceiling(Corner::NE), ceiling(Corner::SE), east.ceiling(Corner::NW), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableEast), _room }, (~SectorFlag::Climbable | SectorFlag::ClimbableWest), Vector3::Right);
        }

        if (west && (west.is_portal() || !west.is_wall()) && (ceiling(Corner::NW) != west.ceiling(Corner::NE) || ceiling(Corner::SW) != west.ceiling(Corner::SE)))
        {
            add_quad_remote(self, west, Quad{ ceiling(Corner::SW), west.ceiling(Corner::NE), west.ceiling(Corner::SE), ceiling(Corner::NW), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableWest), _room }, (~SectorFlag::Climbable | SectorFlag::ClimbableEast), Vector3::Left);
        }
    }

    void Sector::generate_outsides(const Portal& self, const Portal& north, const Portal& east, const Portal& south, const Portal& west)
    {
        const SectorFlag wall_flags = _flags & ~(SectorFlag::Death | SectorFlag::MonkeySwing);
        if (is_wall())
        {
            if (north && !north.is_wall() && !north.is_portal())
            {
                add_quad(self, Quad(north.ceiling(Corner::SE), north.corner(Corner::SW), north.corner(Corner::SE), north.ceiling(Corner::SW), (wall_flags & ~SectorFlag::Climbable) | (SectorFlag::ClimbableSouth & north.direct->flags()), _room));
            }

            if (south && !south.is_wall() && !south.is_portal())
            {
                add_quad(self, Quad(south.ceiling(Corner::NW), south.corner(Corner::NE), south.corner(Corner::NW), south.ceiling(Corner::NE), (wall_flags & ~SectorFlag::Climbable) | (SectorFlag::ClimbableNorth & south.direct->flags()), _room));
            }

            if (east && !east.is_wall() && !east.is_portal())
            {
                add_quad(self, Quad(east.ceiling(Corner::SW), east.corner(Corner::NW), east.corner(Corner::SW), east.ceiling(Corner::NW), (wall_flags & ~SectorFlag::Climbable) | (SectorFlag::ClimbableWest & east.direct->flags()), _room));
            }

            if (west && !west.is_wall() && !west.is_portal())
            {
                add_quad(self, Quad(west.ceiling(Corner::NE), west.corner(Corner::SE), west.corner(Corner::NE), west.ceiling(Corner::SE), (wall_flags & ~SectorFlag::Climbable) | (SectorFlag::ClimbableEast & west.direct->flags()), _room));
            }
        }
        else
        {
            if (north && (north.is_portal() || !has_flag(north.flags(), SectorFlag::Wall)))
            {
                if (north.is_portal() && !north.target)
                {
                    add_quad(self, Quad(ceiling(Corner::NW), corner(Corner::NE), corner(Corner::NW), ceiling(Corner::NE), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableNorth), _room));
                }
                else if (corner(Corner::NW) != north.corner(Corner::SW) || corner(Corner::NE) != north.corner(Corner::SE))
                {
                    Quad quad{ north.corner(Corner::SW), corner(Corner::NE), corner(Corner::NW), north.corner(Corner::SE), wall_flags, _room };
                    if (quad.triangles()[0].normal == Vector3::Backward)
                    {
                        const auto flags = north.target ? north.target->flags() : north.flags();
                        const SectorFlag north_wall_flags = flags & ~(SectorFlag::Death | SectorFlag::MonkeySwing);
                        quad.type = north_wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableSouth);
                        add_quad(north, quad);
                    }
                    else
                    {
                        quad.type = wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableNorth);
                        add_quad(self, quad);
                    }
                }
            }

            if (south && (south.is_portal() || !has_flag(south.flags(), SectorFlag::Wall)))
            {
                if (south.is_portal() && !south.target)
                {
                    add_quad(self, Quad(corner(Corner::SW), ceiling(Corner::SE), ceiling(Corner::SW), corner(Corner::SE), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableSouth), _room));
                }
                else if (corner(Corner::SW) != south.corner(Corner::NW) || corner(Corner::SE) != south.corner(Corner::NE))
                {
                    Quad quad{ corner(Corner::SW), south.corner(Corner::NE), south.corner(Corner::NW), corner(Corner::SE), wall_flags, _room };
                    if (quad.triangles()[0].normal == Vector3::Forward)
                    {
                        const auto flags = south.target ? south.target->flags() : south.flags();
                        const SectorFlag south_wall_flags = flags & ~(SectorFlag::Death | SectorFlag::MonkeySwing);
                        quad.type = south_wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableNorth);
                        add_quad(south, quad);
                    }
                    else
                    {
                        quad.type = wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableSouth);
                        add_quad(self, quad);
                    }
                }
            }

            if (east && (east.is_portal() || !has_flag(east.flags(), SectorFlag::Wall)))
            {
                if (east.is_portal() && !east.target)
                {
                    add_quad(self, Quad(corner(Corner::SE), ceiling(Corner::NE), ceiling(Corner::SE), corner(Corner::NE), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableEast), _room));
                }
                else if (corner(Corner::NE) != east.corner(Corner::NW) || corner(Corner::SE) != east.corner(Corner::SW))
                {
                    Quad quad{ corner(Corner::SE), east.corner(Corner::NW), east.corner(Corner::SW), corner(Corner::NE), wall_flags, _room };
                    if (quad.triangles()[0].normal == Vector3::Right)
                    {
                        const auto flags = east.target ? east.target->flags() : east.flags();
                        const SectorFlag east_wall_flags = flags & ~(SectorFlag::Death | SectorFlag::MonkeySwing);
                        quad.type = east_wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableWest);
                        add_quad(east, quad);
                    }
                    else
                    {
                        quad.type = wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableEast);
                        add_quad(self, quad);
                    }
                }
            }

            if (west && (west.is_portal() || !has_flag(west.flags(), SectorFlag::Wall)))
            {
                if (west.is_portal() && !west.target)
                {
                    add_quad(self, Quad(ceiling(Corner::SW), corner(Corner::NW), corner(Corner::SW), ceiling(Corner::NW), wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableWest), _room));
                }
                else if (corner(Corner::NW) != west.corner(Corner::NE) || corner(Corner::SW) != west.corner(Corner::SE))
                {
                    Quad quad{ west.corner(Corner::SE), corner(Corner::NW), corner(Corner::SW), west.corner(Corner::NE), wall_flags, _room };
                    if (quad.triangles()[0].normal == Vector3::Left)
                    {
                        const auto flags = west.target ? west.target->flags() : west.flags();
                        const SectorFlag west_wall_flags = flags & ~(SectorFlag::Death | SectorFlag::MonkeySwing);
                        quad.type = west_wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableEast);
                        add_quad(west, quad);
                    }
                    else
                    {
                        quad.type = wall_flags & (~SectorFlag::Climbable | SectorFlag::ClimbableWest);
                        add_quad(self, quad);
                    }
                }
            }
        }
    }

    int8_t Sector::floor() const
    {
        return _floor;
    }

    int8_t Sector::ceiling() const
    {
        return _ceiling;
    }

    uint16_t Sector::material() const
    {
        return _material;
    }

    uint16_t Sector::box_index() const
    {
        return _box_index;
    }

    bool Sector::stopper() const
    {
        return _stopper;
    }

    int32_t Sector::filterable_index() const
    {
        return static_cast<int32_t>(_sector_id);
    }
}

