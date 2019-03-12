#define NOMINMAX
#include "Sector.h"

namespace trview
{
    Sector::Sector(const trlevel::ILevel &level, const trlevel::tr3_room& room, const trlevel::tr_room_sector &sector, int sector_id, uint32_t room_number)
        : _level(level), _sector(sector), _sector_id(sector_id), _room_above(sector.room_above), _room_below(sector.room_below), _room(room_number)
    {
        _x = sector_id / room.num_z_sectors;
        _z = sector_id % room.num_z_sectors;
        parse();
    }

    std::uint16_t
    Sector::portal() const
    {
        if (!(flags & SectorFlag::Portal))
            throw std::runtime_error("Sector does not have portal function");

        return _portal;
    }

    std::set<std::uint16_t> 
    Sector::neighbours() const
    {
        std::set<std::uint16_t> neighbours; 

        const auto add_neighbour = [&] (std::uint16_t room)
        {
            const auto &r = _level.get_room(room); 
            if (r.alternate_room != -1)
                neighbours.insert(r.alternate_room); 

            neighbours.insert(room); 
        }; 

        if (flags & SectorFlag::Portal)
            add_neighbour(_portal);
        if (flags & SectorFlag::RoomAbove)
            add_neighbour(_room_above); 
        if (flags & SectorFlag::RoomBelow)
            add_neighbour(_room_below); 

        return neighbours; 
    }

    bool
    Sector::parse()
    {
        // Basic sector items 
        if (_sector.floor == -127 && _sector.ceiling == -127)
            flags |= SectorFlag::Wall;
        if (_room_above != 0xFF)
            flags |= SectorFlag::RoomAbove;
        if (_room_below != 0xFF)
            flags |= SectorFlag::RoomBelow; 

        // Start off the heights at the height of the floor (or in the case of a 
        // wall, at the bottom of the room).
        _corners.fill(flags & SectorFlag::Wall ?
            _level.get_room(_room).info.yBottom / trlevel::Scale_Y :
            _sector.floor * 0.25f);

        std::uint16_t cur_index = _sector.floordata_index;
        if (cur_index == 0x0)
            return true; 

        const auto max_floordata = _level.num_floor_data();

        for (;;)
        {
            std::uint16_t floor = _level.get_floor_data(cur_index);
            std::uint16_t subfunction = (floor & 0x7F00) >> 8; 

            switch (floor & 0x1f)
            {
            case 0x1:
                _portal = _level.get_floor_data(++cur_index) & 0xFF;
                flags |= SectorFlag::Portal;
                break; 

            case 0x2: 
            {
                _floor_slant = _level.get_floor_data(++cur_index);
                flags |= SectorFlag::FloorSlant;
                parse_slope();
                break;
            }
            case 0x3:
                _ceiling_slant = _level.get_floor_data(++cur_index);
                flags |= SectorFlag::CeilingSlant;
                break;

            case 0x4:
            {
                std::uint16_t command = 0; 
                std::uint16_t setup = _level.get_floor_data(++cur_index);

                // Basic trigger setup 
                _trigger.timer = setup & 0xFF;
                _trigger.oneshot = (setup & 0x100) >> 8; 
                _trigger.mask = (setup & 0x3E00) >> 9; 
                _trigger.sector_id = _sector_id;

                // Type of the trigger, e.g. Pad, Switch, etc.
                _trigger.type = (TriggerType) subfunction;

                if (_trigger.type == TriggerType::Key || _trigger.type == TriggerType::Switch)
                {
                    // The next element is the lock or switch - ignore.
                    ++cur_index;
                }

                // Parse actions 
                do
                {
                    if (++cur_index < max_floordata)
                    {
                        command = _level.get_floor_data(cur_index);
                        auto action = static_cast<TriggerCommandType>((command & 0x7C00) >> 10);
                        _trigger.commands.emplace_back(action, command & 0x3FF);
                        if (action == TriggerCommandType::Camera)
                        {
                            // Camera has another uint16_t - skip for now.
                            command = _level.get_floor_data(++cur_index);
                        }
                    }

                } while (cur_index < max_floordata && !(command & 0x8000));

                flags |= SectorFlag::Trigger; 
                break; 
            }
            case 0x5: 
                flags |= SectorFlag::Death; 
                break; 

            case 0x6: // climbable walls 
                flags |= (subfunction << 6); 
                break; 

            case 0x7:
            case 0xB:
            case 0xC:
            case 0x8:
            case 0xD:
            case 0xE:
            {
                // Not sure what to do with h1 and h2 values yet.
                const int16_t h2 = (floor & 0x7C00) >> 10;
                const int16_t h1 = (floor & 0x03E0) >> 5;
                const int16_t function = (floor & 0x001F);

                switch (function)
                {
                case 0x07:
                case 0x0B:
                case 0x0C:
                    _triangulation_function = TriangulationDirection::NwSe;
                    break;
                case 0x08:
                case 0x0D:
                case 0x0E:
                    _triangulation_function = TriangulationDirection::NeSw;
                    break;
                }

                const uint16_t corner_values = _level.get_floor_data(++cur_index);
                const uint16_t c00 = (corner_values & 0x00F0) >> 4;
                const uint16_t c01 = (corner_values & 0x0F00) >> 8;
                const uint16_t c10 = (corner_values & 0x000F);
                const uint16_t c11 = (corner_values & 0xF000) >> 12;
                const auto max_corner = std::max({ c00, c01, c10, c11 });

                _corners[0] += (max_corner - c00) * 0.25f;
                _corners[1] += (max_corner - c01) * 0.25f;
                _corners[2] += (max_corner - c10) * 0.25f;
                _corners[3] += (max_corner - c11) * 0.25f;
                break;
            }
            case 0x9:
            case 0xA:
            case 0xF:
            case 0x10:
            case 0x11:
            case 0x12:
            {
                // Ceiling triangulation.
                _level.get_floor_data(++cur_index);
                break;
            }
            case 0x13: 
                flags |= SectorFlag::MonkeySwing;
                break; 
            case 0x14:
                flags |= SectorFlag::MinecartLeft;
                break; 
            case 0x15:
                flags |= SectorFlag::MinecartRight; 
                break; 
            }

            if ((floor >> 15) || cur_index == 0x0) break; 
            else cur_index++; 
        }

        return true; 
    }

    const TriggerInfo& Sector::trigger() const
    {
        return _trigger;
    }

    uint16_t Sector::x() const
    {
        return _x;
    }

    uint16_t Sector::z() const
    {
        return _z;
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

    std::array<float, 4> Sector::corners() const
    {
        return _corners;
    }

    uint32_t Sector::room() const
    {
        return _room;
    }

    TriangulationDirection Sector::triangulation_function() const
    {
        return _triangulation_function;
    }

    std::vector<DirectX::SimpleMath::Vector3> Sector::triangles() const
    {
        using namespace DirectX::SimpleMath;
        const float x = _x + 0.5f;
        const float z = _z + 0.5f;

        if (_triangulation_function == TriangulationDirection::NwSe)
        {
            return
            {
                Vector3(x + 0.5f, _corners[2], z - 0.5f), Vector3(x - 0.5f, _corners[0], z - 0.5f), Vector3(x - 0.5f, _corners[1], z + 0.5f),
                Vector3(x - 0.5f, _corners[1], z + 0.5f), Vector3(x + 0.5f, _corners[3], z + 0.5f), Vector3(x + 0.5f, _corners[2], z - 0.5f)
            };
        }

        return
        {
            Vector3(x + 0.5f, _corners[3], z + 0.5f), Vector3(x - 0.5f, _corners[0], z - 0.5f), Vector3(x - 0.5f, _corners[1], z + 0.5f),
            Vector3(x + 0.5f, _corners[3], z + 0.5f), Vector3(x + 0.5f, _corners[2], z - 0.5f), Vector3(x - 0.5f, _corners[0], z - 0.5f)
        };
    }

    bool Sector::is_floor() const
    {
        return room_below() == 0xff && !(flags & SectorFlag::Wall) && !(flags & SectorFlag::Portal);
    }
}

