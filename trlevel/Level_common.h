#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>
#include <spanstream>

#include <trview.common/Logs/ILog.h>
#include <trview.common/Logs/Activity.h>

#include "trtypes.h"
#include "ILevel.h"

namespace trlevel
{
    /* Common file reading functions */

    template <typename T>
    T peek(std::basic_ispanstream<uint8_t>& file);

    template <typename T>
    void read(std::basic_ispanstream<uint8_t>& file, T& value);

    template <typename T>
    T read(std::basic_ispanstream<uint8_t>& file);

    std::vector<uint8_t> read_compressed(std::basic_ispanstream<uint8_t>& file);

    template < typename DataType >
    std::vector<DataType> read_vector_compressed(std::basic_ispanstream<uint8_t>& file, uint32_t elements);

    template < typename DataType, typename SizeType >
    std::vector<DataType> read_vector(std::basic_ispanstream<uint8_t>& file, SizeType size);

    template < typename SizeType, typename DataType >
    std::vector<DataType> read_vector(std::basic_ispanstream<uint8_t>& file);

    void skip(std::basic_ispanstream<uint8_t>& file, uint32_t size);

    template < typename DataType, typename SizeType >
    void stream_vector(std::basic_ispanstream<uint8_t>& file, SizeType size, const std::function<void(const DataType&)>& out);

    /* Logging */

    void log_file(trview::Activity& activity, std::basic_ispanstream<uint8_t>& stream, const std::string& text);
    void log_file(trview::Activity& activity, std::istream& stream, const std::string& text);

    /* Shared level data reading functions */

    bool is_ngle_sound_samples(trview::Activity&, std::basic_ispanstream<uint8_t>& file);
    std::vector<tr4_ai_object> read_ai_objects(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint16_t> read_animated_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    uint8_t read_animated_textures_uv_count(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_animation> read_animations_tr1_3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr4_animation> read_animations_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_anim_command> read_anim_commands(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_anim_dispatch> read_anim_dispatches(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr2_box> read_boxes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_camera> read_cameras(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_cinematic_frame> read_cinematic_frames(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint8_t> read_demo_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr2_entity> read_entities(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint16_t> read_floor_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr4_flyby_camera> read_flyby_cameras(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_fog_bulbs_tr5_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room, uint32_t num_fog_bulbs);
    std::vector<uint16_t> read_frames(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint8_t> read_light_map(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint16_t> read_mesh_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint32_t> read_mesh_pointers(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_model> read_models_tr1_4(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_model> read_models_tr5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint32_t> read_meshtree(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    uint32_t read_num_data_words(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file);
    std::vector<tr_object_texture> read_object_textures_tr1_3(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_object_texture> read_object_textures_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_object_texture> read_object_textures_tr5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint16_t> read_overlaps(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_room_alternate_group(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_alternate_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_ambient_intensity_1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_colour(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_flags(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    tr_room_info read_room_info(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file);
    std::vector<tr5_room_layer> read_room_layers(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr5_room_header& header);
    void read_room_light_mode(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_lights_tr5_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room, uint16_t num_lights);
    void read_room_portals(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_rectangles(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_reverb_info(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_sectors(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_sectors_tr5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_sprites(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_static_meshes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_static_meshes_tr5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room, const tr5_room_header& header);
    void read_room_triangles(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_vertices_tr3_4(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    void read_room_water_scheme(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room);
    template <typename size_type>
    std::vector<tr3_room> read_rooms(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, std::function<void(trview::Activity& activity, std::basic_ispanstream<uint8_t>&, tr3_room&)> load_function);
    std::vector<uint32_t> read_sample_indices(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<uint8_t> read_sound_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_x_sound_details> read_sound_details(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_sound_samples_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<int16_t> read_sound_map(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_sound_source> read_sound_sources(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_sprite_sequence> read_sprite_sequences(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_sprite_texture> read_sprite_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::vector<tr_state_change> read_state_changes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    std::unordered_map<uint32_t, tr_staticmesh> read_static_meshes(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    uint32_t read_textiles(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    uint32_t read_textiles_tr4_5(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks);
    void read_zones(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes);
    void skip_xela(std::basic_ispanstream<uint8_t>& file);
}

#include "Level_common.inl"
