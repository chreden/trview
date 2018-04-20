#pragma once

#include "RoomInfo.h"
#include "trlevel/ILevel.h"

#include "MapTypes.h"

namespace trview
{
    class Map
    {
    public:
        // Initialise object with the specified level and room
        Map(const trlevel::ILevel& level, const trlevel::tr3_room&);
        
        // Generates floor data
        // If the level and room have not changed since the last generation, then this does not 
        // regenerate and instead just returns _data.
        std::vector<FloorData> generate();

        // Loads the level and room specified
        void load(const trlevel::ILevel&, const trlevel::tr3_room&);

        // Returns the number of columns in the room (equal to number of Z sectors) 
        inline std::uint16_t columns() const { return _count_columns; }

        // Returns the number of rows in the room (equal to number of X sectors) 
        inline std::uint16_t rows() const { return _count_rows; }

    private:
        // Returns the floor data at the specified sector index 
        FloorData get_floor_data_at_sector_index(std::uint16_t, std::uint16_t, std::uint16_t) const;

        trlevel::tr3_room           _room;
        std::vector<std::uint16_t>  _raw_floor_data;
        std::uint16_t               _count_columns = 0, _count_rows = 0;
        bool                        _is_dirty = true; // flag to say whether new room has been loaded, so regeneration required
        std::vector<FloorData>      _data; 
    };
}