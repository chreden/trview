export module trlevel:TR1;

import std;
import std.compat;

import trview.common;

import :Types;
import :ILevel;

namespace trlevel
{
    export std::vector<tr_box> read_boxes_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    export std::vector<tr2_entity> read_entities_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    export void read_room_vertices_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    export void read_zones_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes);
}
