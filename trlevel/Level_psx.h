#pragma once

#include <cstdint>
#include <vector>
#include <spanstream>

#include <trview.common/Logs/Activity.h>

#include "trtypes.h"
#include "ILevel.h"

namespace trlevel
{
    std::vector<uint8_t> convert_vag_to_wav(const std::vector<uint8_t>& bytes, uint32_t sample_frequency);
    std::vector<tr_model> read_models_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_sounds_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency);
    void read_sounds_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency);
    uint16_t attribute_for_object_texture(const tr_object_texture_psx& texture, const tr_clut& clut);
    std::vector<tr3_room> read_rooms_tr4_psx(uint16_t num_rooms, std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks);
    void read_sounds_tr4_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sounds_offset, uint32_t data_offset, uint32_t num_sounds, uint32_t data_length, uint32_t sample_frequency);
}
