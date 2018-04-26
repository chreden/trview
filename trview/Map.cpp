#include "stdafx.h"
#include "Map.h"

namespace trview
{
    Map::Map(const trlevel::ILevel& level, const trlevel::tr3_room& room)
    {
        load(level, room);
    }

    std::vector<Sector>
    Map::generate()
    {
        // haven't loaded a new room so no need to regenerate 
        if (!_is_dirty && !_sectors.empty()) return _sectors;

        _sectors.clear();
        _sectors.reserve(_count_columns * _count_rows);

        for (uint16_t column = 0; column < _count_columns; ++column)
        {
            for (uint16_t row = 0; row < _count_rows; ++row)
            {
                std::uint16_t sector_index = (_count_rows * column) + row;
                Sector data = parse(sector_index, _count_rows - row - 1, column);

                _sectors.push_back(data);
            }
        }

        _is_dirty = false;
        return _sectors;
    }

    void
    Map::load(const trlevel::ILevel& level, const trlevel::tr3_room& room)
    {
        _room = room;
        _raw_floor_data = level.get_floor_data_all();
        _count_columns = room.num_x_sectors;
        _count_rows = room.num_z_sectors;
        _is_dirty = true;
    }

    Sector
    Map::parse(std::uint16_t sector_index, std::uint16_t row, std::uint16_t column) const
    {
        Sector data(row, column); 

        trlevel::tr_room_sector sector;
        std::uint16_t cur_index;
        bool end_data = false;     

        if (sector_index <= area()
            && sector_index < _room.sector_list.size())
        {
            sector = _room.sector_list[sector_index];
            data.floor = sector.floor; 
            data.ceiling = sector.ceiling; 

            cur_index = sector.floordata_index; 

            // This continues to read the *next* floordata entry until we receive the enddata flag
            // The enddata flag is (floordata & 0x8000) > 0
            while (!end_data && cur_index != 0)
            {
                std::uint16_t floor = _raw_floor_data.at(cur_index); 
                if (!floor) break; // Floor data 0 means dummy data, no useful info 

                // https://opentomb.earvillage.net/OpenTomb/doc/trosettastone.html#FloorData
                // Function: bits 0..4 (0x001F) => type of action
                // Subfunction: bits 8..14 (0x7F00) => action's conditions and case switches 
                // EndData: bit 15 (0x8000) => if != 0, end of data - stop parsing 
                std::uint16_t function = floor & 0x1F;
                std::uint8_t sub_function = (floor & 0x7F00) >> 8;
                end_data = (floor >> 15); 

                Floor floor_data((FunctionType) function);

                switch (floor_data.function)
                {
                case FunctionType::PORTAL:
                    floor_data.portal_to = _raw_floor_data.at(++cur_index); 
                    break; 

                case FunctionType::FLOOR_SLANT:
                    ++cur_index;
                    break; 

                case FunctionType::CEILING_SLANT:
                    ++cur_index;
                    break; 

                case FunctionType::TRIGGER:
                    ++cur_index;
                    _raw_floor_data.at(++cur_index); 
                    while (!(_raw_floor_data.at(++cur_index) >> 15))
                        ++cur_index; 

                    break; 

                case FunctionType::KILL:
                    break; 

                case FunctionType::CLIMBABLE:
                    break; 
                }

                data.add(floor_data);
                cur_index++; 
            };

            if (data.size() == 0)
                data.add(Floor(FunctionType::NORMAL));
        }


       
        return data;
    }
}