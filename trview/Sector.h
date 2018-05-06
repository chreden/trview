#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <set>

#include "trlevel/trtypes.h"
#include "trlevel/ILevel.h"
#include "RoomInfo.h"
#include "Types.h" 

namespace trview
{
    class Sector
    {
    public:
        // Constructs sector object and parses floor data automatically 
        Sector(const trlevel::ILevel &level, const trlevel::tr_room_sector &sector, int sector_id);

        // Returns the id of the room that this floor data points to 
        std::uint16_t portal() const; 

        // Gets/sets id of the sector. Used by map renderer. 
        inline std::uint16_t id() const { return _sector_id; }

        // Returns all neighbours for the current sector, maximum of 3 (up, down, portal). 
        std::set<std::uint16_t> neighbours() const; 

        // Returns room below 
        inline std::uint16_t room_below() const { return _room_below; }

        // Returns room above 
        inline std::uint16_t room_above() const { return _room_above; }

        // Holds "Function" enum bitwise values 
        std::uint16_t flags = 0;

    private:
        bool parse(); 

        // Holds the "wall portal" that this sector points to - this is the id of the room 
        std::uint8_t _portal, _room_above, _room_below;

        // Holds slope data 
        std::uint16_t _floor_slant, _ceiling_slant; 

        // Holds trigger data 
        struct Trigger _trigger; 

        // ID of the sector 
        std::uint16_t _sector_id; 

        // Reference to the level this sector belongs to 
        const trlevel::ILevel &_level;

        // Reference to the base sector structure 
        const trlevel::tr_room_sector &_sector;
    };
}