export module trlevel:TR3;

import std;
import std.compat;

import trview.common;

import :Types;
import :ILevel;

namespace trlevel
{
    void read_room_lights_tr3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
}
