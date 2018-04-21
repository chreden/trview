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

        for (int column = 0; column < _count_columns; ++column)
        {
            for (int row = 0; row < _count_rows; ++row)// row >= 0; --row)
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
        Sector data; 
        trlevel::tr_room_sector sector;
        std::uint16_t start_index, cur_index; 
        bool end_data = false; 

        if (sector_index <= area()
            && sector_index < _room.sector_list.size())
        {
            sector = _room.sector_list[sector_index];
            data.floor = sector.floor; 
            data.ceiling = sector.ceiling; 

            if (data.floor == -127 && data.ceiling == -127)
            {
                data.floor_data.push_back(Function::WALL);
            }
            else
            {
                // This continues to read the *next* floordata entry until we receive the enddata flag
                // The enddata flag is (floordata & 0x8000) > 0
                for (cur_index = sector.floordata_index; !end_data && cur_index != 0; ++cur_index)
                {
                    // No bounds check on this 
                    // If this fails I want it to be caught early rather than silently failing, at() throws out_of_range
                    std::uint16_t header = _raw_floor_data.at(cur_index);
                    if (header == 0x0) break;

                    // https://opentomb.earvillage.net/OpenTomb/doc/trosettastone.html#FloorData
                    // Function: bits 0..4 (0x001F) => type of action
                    // Subfunction: bits 8..14 (0x7F00) => action's conditions and case switches 
                    // EndData: bit 15 (0x8000) => if != 0, end of data - stop parsing 
                    std::uint8_t function = header & 0x1F;
                    std::uint16_t sub_function = (header & 0x7F00) >> 8;
                    end_data = true;

                    // Handle function specific items 
                    Floor flr((Function)function);
                    data.floor_data.push_back(flr);
                }
            }

            if (data.floor_data.empty())
            {
                data.floor_data.push_back(Function::NORMAL);
            }
         
        }

        data.row = row; 
        data.column = column; 
       
        return data;
    }
}