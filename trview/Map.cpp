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
                auto data = get_floor_data_at_sector_index(sector_index, _count_rows - row, column);

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
    Map::get_floor_data_at_sector_index(std::uint16_t sector_index, std::uint16_t row, std::uint16_t column) const
    {
        FloorData data;
        trlevel::tr_room_sector sector; 
        std::uint16_t floor; 

        if (sector_index > 0
            && sector_index <= (_room.num_x_sectors * _room.num_z_sectors))
        {
            sector = _room.sector_list.at(sector_index);
            floor = _raw_floor_data.at(sector.floordata_index);

            data.floor = sector.floor;
            data.ceiling = sector.ceiling;

            data.function = (Function) (floor & 0x1F); 
            data.subfunction = (floor & 0x7F00);
        }

        data.row = row; 
        data.column = column; 
       
        return data;
    }
}