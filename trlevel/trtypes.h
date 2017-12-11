#pragma once

#include <cstdint>

namespace trlevel
{
    struct tr_colour
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
    };

    struct tr_textile8
    {
        uint8_t Tile[256 * 256];
    };

    struct tr_textile16
    {
        uint16_t Tile[256 * 256];
    };

    struct tr_textile32
    {
        uint32_t Tile[256 * 256];
    };
}