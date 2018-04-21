#include "stdafx.h"
#include "Map.h"

namespace trview
{
    Map::Map(const trlevel::ILevel& level, const trlevel::tr3_room& room)
    {
        load(level, room);
    }

    std::vector<FloorData>
    Map::generate()
    {
        // haven't loaded a new room so no need to regenerate 
        if (!_is_dirty && !_data.empty()) return _data;

        _data.clear();
        _data.reserve(_count_columns * _count_rows);

        for (int column = 0; column < _count_columns; ++column)
        {
            for (int row = 0; row < _count_rows; ++row)// row >= 0; --row)
            {
                std::uint16_t sector_index = (_count_rows * column) + row;
                FloorData data = parse(sector_index, _count_rows - row - 1, column);

                _data.push_back(data);
            }
        }

        _is_dirty = false;
        return _data;
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

    FloorData
    Map::parse(std::uint16_t sector_index, std::uint16_t row, std::uint16_t column) const
    {
        FloorData data; 
        trlevel::tr_room_sector sector;
        std::uint16_t start_index, cur_index; 
        bool end_data = false; 

        if (sector_index <= area()
            && sector_index < _room.sector_list.size())
        {
            sector = _room.sector_list[sector_index];
            data.floor = sector.floor; 
            data.ceiling = sector.ceiling; 

            // This continues to read the *next* floordata entry until we receive the enddata flag
            // The enddata flag is (floordata & 0x8000) > 0
            for (cur_index = sector.floordata_index; !end_data; ++cur_index)
            {
                // No bounds check on this 
                // If this fails I want it to be caught early rather than silently failing, at() throws out_of_range
                std::uint16_t header = _raw_floor_data.at(cur_index); 

                // https://opentomb.earvillage.net/OpenTomb/doc/trosettastone.html#FloorData
                // Function: bits 0..4 (0x001F) => type of action
                // Subfunction: bits 8..14 (0x7F00) => action's conditions and case switches 
                // EndData: bit 15 (0x8000) => if != 0, end of data - stop parsing 
                std::uint8_t function = header & 0x1F;
                std::uint16_t sub_function = (header & 0x7F00) >> 8; 
                end_data = (bool) ((header & 0x8000) >> 15);
                
                // Handle function specific items 
                Function func((FunctionType) function);
                data.functions.push_back(func); 
            }
        }

        data.row = row; 
        data.column = column; 
       
        return data;
    }
}