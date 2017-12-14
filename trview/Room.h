#pragma once

#include "RoomInfo.h"

namespace trview
{
    class Room
    {
    public:
        Room(int32_t x, int32_t z, int32_t yBottom, int32_t yTop);

        RoomInfo info() const;
    private:
        RoomInfo _info;
    };
}