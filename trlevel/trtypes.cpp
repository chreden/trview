#include "trtypes.h"

namespace trlevel
{
    uint32_t convert_textile16(uint16_t t)
    {
        uint16_t r = t & 0x001f;
        uint16_t g = (t & 0x03e0) >> 5;
        uint16_t b = (t & 0x7c00) >> 10;

        r <<= 3;
        g <<= 3;
        b <<= 3;

        r += 3;
        g += 3;
        b += 3;

        return 0xff << 24 | r << 16 | g << 8 | b;
    }
}