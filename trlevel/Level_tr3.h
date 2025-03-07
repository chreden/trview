#pragma once

#include <cstdint>
#include <spanstream>

#include <trview.common/Logs/Activity.h>

#include "trtypes.h"
#include "ILevel.h"

namespace trlevel
{
    void read_room_lights_tr3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
}
