export module trlevel:TR2;

import std;
import std.compat;

import trview.common;

import :Types;
import :ILevel;

namespace trlevel
{
    void read_room_ambient_intensity_2(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_lights_tr2_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
}
