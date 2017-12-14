#pragma once

#include <string>
#include <vector>

#include "ILevel.h"
#include "trtypes.h"

namespace trlevel
{
    class Level : public ILevel
    {
    public:
        explicit Level(std::wstring filename);

        virtual ~Level();

        // Gets the number of textiles in the level.
        // Returns: The number of textiles.
        virtual uint32_t num_textiles() const override;

        // Gets the 8 bit textile with the specified index.
        // Returns: The textile for this index.
        virtual tr_textile8 get_textile8(uint32_t index) const override;

        // Gets the 16 bit textile with the specified index.
        // Returns: The textile for this index.
        virtual tr_textile16 get_textile16(uint32_t index) const override;

        // Gets the number of rooms in the level.
        // Returns: The number of rooms.
        virtual uint16_t num_rooms() const override;

        // Get the room at the specified index.
        // Returns: The room.
        virtual tr3_room get_room(uint16_t index) const override;
    private:
        uint32_t                  _num_textiles;
        std::vector<tr_textile8>  _textile8;
        std::vector<tr_textile16> _textile16;

        uint16_t                  _num_rooms;
        std::vector<tr3_room>     _rooms;
    };
}
