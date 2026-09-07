export module trview.common:Resource;

import std;
import std.compat;

namespace trview
{
    export struct Resource
    {
        uint8_t* data;
        uint32_t size;
    };
}
