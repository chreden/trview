#include "Level.h"
#include "Level_common.h"
#include "Level_tr2.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
        void read_room_vertices_tr2(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading vertices");
            room.data.vertices = convert_vertices(read_vector<int16_t, tr2_room_vertex>(file));
            log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));
        }

        void load_tr2_pc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);

            // Read actual room data.
            if (NumDataWords > 0)
            {
                read_room_vertices_tr2(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);
                read_room_sprites(activity, file, room);
            }

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_ambient_intensity_2(activity, file, room);
            read_room_light_mode(activity, file, room);
            read_room_lights_tr2_pc(activity, file, room);
            read_room_static_meshes(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
        }

        uint32_t convert_textile16_pc_e3(uint16_t t)
        {
            uint16_t r = (t & 0x7c00) >> 10;
            uint16_t g = (t & 0x03e0) >> 5;
            uint16_t b = t & 0x001f;

            r = static_cast<uint16_t>((r / 31.0f) * 255.0f);
            g = static_cast<uint16_t>((g / 31.0f) * 255.0f);
            b = static_cast<uint16_t>((b / 31.0f) * 255.0f);

            if ((r == 255 && b == 255 && g == 0) ||
                (r == 0 && b == 0 && g == 0))
            {
                return 0x00000000;
            }
            return 0xff000000 | b << 16 | g << 8 | r;
        }

        std::vector<uint32_t> convert_textile_pc_e3(const tr_textile16& tile)
        {
            return tile.Tile | std::views::transform(convert_textile16_pc_e3) | std::ranges::to<std::vector>();
        }
    }

    void read_room_ambient_intensity_2(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading ambient intensity 2");
        room.ambient_intensity_2 = read<int16_t>(file);
        log_file(activity, file, std::format("Read ambient intensity 2: {}", room.ambient_intensity_2));
    }

    void read_room_lights_tr2_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading lights");
        room.lights = convert_lights(read_vector<uint16_t, tr2_room_light>(file));
        log_file(activity, file, std::format("Read {} lights", room.lights.size()));
    }

    void Level::read_textiles_tr2_pc_e3(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");

        uint32_t num_textiles = read<uint32_t>(file);
        callbacks.on_progress(std::format("Skipping {} 8-bit textiles", num_textiles));
        log_file(activity, file, std::format("Skipping {} 8-bit textiles", num_textiles));
        skip(file, sizeof(tr_textile8) * num_textiles);

        callbacks.on_progress(std::format("Reading {} 16-bit textiles", num_textiles));
        log_file(activity, file, std::format("Reading {} 16-bit textiles", num_textiles));
        stream_vector<tr_textile16>(file, num_textiles, [&](auto&& t) { callbacks.on_textile(convert_textile_pc_e3(t)); });
    }

    void Level::load_tr2_pc_e3(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        read_palette_tr1(file, activity, callbacks);
        read_textiles_tr2_pc_e3(file, activity, callbacks);

        read<uint32_t>(file);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr2_pc_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = convert_animations(read_animations_tr1_3(activity, file, callbacks));
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr1_4(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);
        _object_textures = read_object_textures_tr1_3(activity, file, callbacks);
        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        uint32_t box_count = read<uint32_t>(file);
        skip(file, box_count * 20);
        uint32_t overlap_count = read<uint32_t>(file);
        skip(file, overlap_count * 2);
        skip(file, box_count * 20);
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read_light_map(activity, file, callbacks);
        read_cinematic_frames(activity, file, callbacks);
        read_demo_data(activity, file, callbacks);
        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        uint32_t num_sounds = peek<uint32_t>(file);
        for (uint32_t s = 0; s < num_sounds; ++s)
        {
            skip(file, 4); // count/number
            skip(file, 4); // RIFF
            uint32_t size = peek<uint32_t>(file);
            file.seekg(-4, std::ios::cur);
            _sound_samples.push_back(read_vector<uint8_t>(file, size + 4));
        }
        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::load_tr2_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        if (is_tr2_e3(_platform_and_version))
        {
            return load_tr2_pc_e3(file, activity, callbacks);
        }

        skip(file, 4); // version number
        read_palette_tr2_3(file, activity, callbacks);
        read_textiles(activity, file, callbacks);

        read<uint32_t>(file);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr2_pc_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = convert_animations(read_animations_tr1_3(activity, file, callbacks));
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr1_4(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);
        _object_textures = read_object_textures_tr1_3(activity, file, callbacks);
        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read_light_map(activity, file, callbacks);
        read_cinematic_frames(activity, file, callbacks);
        read_demo_data(activity, file, callbacks);
        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        _sample_indices = read_sample_indices(activity, file, callbacks);
        load_sound_fx(activity, callbacks);
        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }
}

