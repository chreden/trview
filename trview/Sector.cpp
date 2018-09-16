#include "stdafx.h"
#include "Sector.h"

namespace trview
{
    Sector::Sector(const trlevel::ILevel &level, const trlevel::tr_room_sector &sector, int sector_id)
        : _level(level), _sector(sector), _sector_id(sector_id), _room_above(sector.room_above), _room_below(sector.room_below)
    {
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
                _floor_slant = _level.get_floor_data(++cur_index); 
                flags |= SectorFlag::FloorSlant; 
                break; 

            case 0x3:
                _ceiling_slant = _level.get_floor_data(++cur_index);
                flags |= SectorFlag::CeilingSlant;
                break;

            case 0x4:
            {
                std::uint16_t command; 
                std::uint16_t setup = _level.get_floor_data(++cur_index);

                // Basic trigger setup 
                _trigger.timer = setup & 0xFF;
                _trigger.oneshot = (setup & 0x100) >> 8; 
                _trigger.mask = (setup & 0x3E00) >> 9; 

                // Type of the trigger, e.g. Pad, Switch, etc.
                _trigger.type = (TriggerType) subfunction;

                // Parse actions 
                ++cur_index;
                while (cur_index < max_floordata && ((command = _level.get_floor_data(cur_index)) & 0x8000))
                {
                    _trigger.commands.emplace_back (
                        (TriggerCommandType) ((command & 0x7C00) >> 10),
                        command & 0x3FF
                    ); 
                    ++cur_index;
                }

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
            case 0x8:
            case 0x9:
            case 0xA:
            case 0xB:
            case 0xC:
            case 0xD:
            case 0xE:
            case 0xF:
            case 0x10:
            case 0x11:
            case 0x12:
                // Triangulation. Don't care about this right now.
                if ((floor & 0x1f) >= 0x7 && (floor & 0x1f) <= 0x12)
                    (void) _level.get_floor_data(++cur_index);
                break;

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
}

