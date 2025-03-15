#pragma once

#include <cstdint>
#include <vector>
#include <spanstream>

#include <trview.common/Logs/Activity.h>

#include "trtypes.h"
#include "ILevel.h"

namespace trlevel
{
    std::vector<tr_model> read_models_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_sounds_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency);
    void read_sounds_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency);
}
