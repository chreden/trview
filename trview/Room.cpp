#include "stdafx.h"
#include "Room.h"

namespace trview
{
    Room::Room(int32_t x, int32_t z, int32_t yBottom, int32_t yTop)
        : _info { x, 0, z, yBottom, yTop }
    {
    }

    RoomInfo Room::info() const
    {
        return _info;
    }
}
