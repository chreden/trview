#pragma once

#include <cstdint>
#include <vector>
#include <spanstream>

#include <trview.common/Logs/Activity.h>

#include "trtypes.h"
#include "ILevel.h"

namespace trlevel
{
    uint16_t attribute_for_object_texture(const tr_object_texture_psx& texture, const tr_clut& clut);
    std::vector<tr_room_vertex> convert_psx_vertex_lighting(std::vector<tr_room_vertex> vertices);
    std::vector<uint8_t> convert_vag_to_wav(const std::vector<uint8_t>& bytes, uint32_t sample_frequency);
    bool is_supported_tr4_psx_version(int32_t version);
    bool is_supported_tr5_psx_version(int32_t version);
    std::vector<tr4_ai_object> read_ai_objects(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<std::vector<int16_t>> read_animated_textures_tr4_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr2_entity> read_entities(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint16_t> read_frames(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, uint32_t start, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint16_t> read_mesh_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint32_t> read_mesh_pointers(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint32_t> read_meshtree(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_model> read_models(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, uint32_t start, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks, uint32_t num_meshes = 460);
    std::vector<tr_model> read_models_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_object_texture_psx> read_object_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    void read_room_geometry_tr4_psx(std::basic_ispanstream<uint8_t>& file, tr3_room& room, uint32_t data_size);
    std::vector<tr_object_texture_psx> read_room_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr3_room> read_rooms_tr4_psx(uint16_t num_rooms, std::basic_ispanstream<uint8_t>& file, LevelVersion version, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_sound_source> read_sound_sources(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks);
    void read_sounds_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency);
    void read_sounds_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency);
    void read_sounds_tr4_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t start, const tr4_psx_level_info& info, uint32_t sample_frequency);
    std::unordered_map<uint32_t, tr_staticmesh> read_static_meshes_tr4_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_statics = 70);
    void read_room_vertices_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
}
