#pragma once

#include <cstdint>
#include <vector>
#include <spanstream>

#include <trview.common/Logs/ILog.h>
#include <trview.common/Logs/Activity.h>

#include "trtypes.h"
#include "ILevel.h"

namespace trlevel
{
    std::vector<tr_box> read_boxes_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr2_entity> read_entities_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_room_vertices_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_zones_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes);
}
