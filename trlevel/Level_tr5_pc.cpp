#include "Level.h"
#include "Level_common.h"

#include <ranges>
#include <format>

namespace trlevel
{
    namespace
    {
        void load_tr5_pc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading room data information");
            skip_xela(file);
            uint32_t room_data_size = read<uint32_t>(file);
            const uint32_t room_start = static_cast<uint32_t>(file.tellg());
            const uint32_t room_end = room_start + room_data_size;
            log_file(activity, file, std::format("Reading room data information. Data Size: {}", room_data_size));

            log_file(activity, file, "Reading room header");
            const auto header = read<tr5_room_header>(file);

            // Copy useful data from the header to the room.
            room.info = header.info;
            room.num_x_sectors = header.num_x_sectors;
            room.num_z_sectors = header.num_z_sectors;
            room.colour = header.colour;
            room.reverb_info = header.reverb_info;
            room.alternate_group = header.alternate_group;
            room.water_scheme = header.water_scheme;
            room.alternate_room = header.alternate_room;
            room.flags = header.flags;
            log_file(activity, file, "Read room header");

            // The offsets start measuring from this position, after all the header information.
            const uint32_t data_start = static_cast<uint32_t>(file.tellg());

            read_room_lights_tr5_pc(activity, file, room, header.num_lights);
            read_fog_bulbs_tr5_pc(activity, file, room, header.num_fog_bulbs);

            file.seekg(data_start + header.start_sd_offset, std::ios::beg);
            read_room_sectors_tr5(activity, file, room);
            read_room_portals(activity, file, room);

            // Separator
            skip(file, 2);
            file.seekg(data_start + header.end_portal_offset, std::ios::beg);
            read_room_static_meshes_tr5(activity, file, room, header);
            file.seekg(data_start + header.layer_offset, std::ios::beg);
            const auto layers = read_room_layers(activity, file, header);

            file.seekg(data_start + header.poly_offset, std::ios::beg);
            uint16_t vertex_offset = 0;
            int32_t layer_number = 0;
            for (const auto& layer : layers)
            {
                log_file(activity, file, std::format("Reading {} rectangles for layer {}", layer.num_rectangles, layer_number));
                auto rects = read_vector<tr4_mesh_face4>(file, layer.num_rectangles);
                for (auto& rect : rects)
                {
                    for (auto& v : rect.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(rects.begin(), rects.end(), std::back_inserter(room.data.rectangles));

                log_file(activity, file, std::format("Reading {} triangles for layer {}", layer.num_triangles, layer_number));
                auto tris = read_vector<tr4_mesh_face3>(file, layer.num_triangles);
                for (auto& tri : tris)
                {
                    for (auto& v : tri.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(tris.begin(), tris.end(), std::back_inserter(room.data.triangles));

                vertex_offset += layer.num_vertices;
                ++layer_number;
            }

            file.seekg(data_start + header.vertices_offset, std::ios::beg);
            layer_number = 0;
            for (const auto& layer : layers)
            {
                log_file(activity, file, std::format("Reading {} vertices for layer {}", layer.num_vertices, layer_number));
                auto verts = convert_vertices(read_vector<tr5_room_vertex>(file, layer.num_vertices));
                std::copy(verts.begin(), verts.end(), std::back_inserter(room.data.vertices));
                ++layer_number;
            }

            file.seekg(room_end, std::ios::beg);
        }

        void load_tr5_pc_remastered_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading room data information");

            log_file(activity, file, "Reading room header");
            const auto header = read<tr5_room_header_remastered>(file);

            room.info.x = header.x;
            room.info.z = header.z;
            room.info.yBottom = header.yBottom;
            room.info.yTop = header.yTop;

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            room.colour = read<uint32_t>(file);

            uint16_t num_lights = read<uint16_t>(file);
            read_room_lights_tr5_pc(activity, file, room, num_lights);
            uint32_t num_fog_bulbs = read<uint32_t>(file);
            read_fog_bulbs_tr5_pc(activity, file, room, num_fog_bulbs);
            read_room_static_meshes(activity, file, room);

            room.alternate_room = read<uint16_t>(file);
            room.flags = read<uint16_t>(file);
            room.water_scheme = read<uint8_t>(file);
            room.reverb_info = read<uint8_t>(file);
            room.alternate_group = read<uint8_t>(file);

            uint16_t vertex_offset = 0;
            uint32_t num_layers = read<uint32_t>(file);
            for (uint32_t i = 0; i < num_layers; ++i)
            {
                const auto layer = read<tr5_room_layer_remastered>(file);

                log_file(activity, file, std::format("Reading {} vertices for layer {}", layer.num_vertices, i));
                auto verts = convert_vertices(read_vector<tr5_room_vertex>(file, layer.num_vertices));
                std::copy(verts.begin(), verts.end(), std::back_inserter(room.data.vertices));

                log_file(activity, file, std::format("Reading {} rectangles for layer {}", layer.num_rectangles, i));
                auto rects = read_vector<tr4_mesh_face4>(file, layer.num_rectangles);
                for (auto& rect : rects)
                {
                    for (auto& v : rect.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(rects.begin(), rects.end(), std::back_inserter(room.data.rectangles));

                log_file(activity, file, std::format("Reading {} triangles for layer {}", layer.num_triangles, i));
                auto tris = read_vector<tr4_mesh_face3>(file, layer.num_triangles);
                for (auto& tri : tris)
                {
                    for (auto& v : tri.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(tris.begin(), tris.end(), std::back_inserter(room.data.triangles));
                vertex_offset += layer.num_vertices;
            }

            // Mystery number - seems to be tied to vertices in some way.
            read<int>(file);
        }

        uint32_t read_textiles_tr5_remastered(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
        {
            log_file(activity, file, "Reading textile counts");
            uint16_t num_room_textiles = read<uint16_t>(file);
            uint16_t num_obj_textiles = read<uint16_t>(file);
            uint16_t num_bump_textiles = read<uint16_t>(file);
            log_file(activity, file, std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
            const auto num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;

            callbacks.on_progress(std::format("Reading {} 32-bit textiles", num_textiles));
            log_file(activity, file, std::format("Reading {} 32-bit textiles", num_textiles));
            skip(file, 4); // skip sizes
            auto textile32 = read_vector<tr_textile32>(file, num_textiles);

            for (const auto& textile : textile32)
            {
                callbacks.on_textile(convert_textile(textile));
            }
            textile32 = {};

            log_file(activity, file, "Skipping misc textiles");
            const auto textile32_misc = read_vector<tr_textile32>(file, 3);
            for (const auto& textile : textile32_misc)
            {
                callbacks.on_textile(convert_textile(textile));
            }
            return num_textiles;
        }
    }

    void Level::load_tr5_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        _num_textiles = read_textiles_tr4_5(activity, file, callbacks);
        log_file(activity, file, "Reading Lara type");
        _lara_type = read<uint16_t>(file);
        log_file(activity, file, std::format("Lara type: {}", _lara_type));
        log_file(activity, file, "Reading weather type");
        _weather_type = read<uint16_t>(file);
        log_file(activity, file, std::format("Weather type: {}", _weather_type));
        log_file(activity, file, "Skipping 28 unknown/padding bytes");
        file.seekg(28, std::ios::cur);

        log_file(activity, file, "Reading uncompressed size and skipping compressed size - unused in Tomb5");
        const auto uncompressed_size = read<uint32_t>(file);
        const auto compressed_size = read<uint32_t>(file);
        compressed_size;
        callbacks.on_progress("Processing level data");
        const auto at = file.tellg();

        // Read unused value.
        read<uint32_t>(file);

        if (file.eof())
        {
            // VICT.TR2 ends here.
            return;
        }

        _rooms = read_rooms<uint32_t>(activity, file, callbacks, load_tr5_pc_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = read_animations_tr4_5(activity, file, callbacks);
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr5(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);

        log_file(activity, file, "Skipping SPR marker");
        // Skip past the 'SPR' marker.
        file.seekg(4, std::ios::cur);

        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _flyby_cameras = read_flyby_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);

        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _animated_texture_uv_count = read_animated_textures_uv_count(activity, file, callbacks);

        log_file(activity, file, "Skipping TEX marker");
        file.seekg(4, std::ios::cur);

        _object_textures = read_object_textures_tr5(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        _ai_objects = read_ai_objects(activity, file, callbacks);
        read_demo_data(activity, file, callbacks);

        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        _sample_indices = read_sample_indices(activity, file, callbacks);

        file.seekg(at + static_cast<std::fpos_t>(uncompressed_size), std::ios::beg);

        const auto sound_start = file.tellg();
        try
        {
            if (is_ngle_sound_samples(activity, file))
            {
                load_ngle_sound_fx(activity, file, callbacks);
            }
            else
            {
                read_sound_samples_tr4_5(file, activity, callbacks);
            }
        }
        catch (const std::exception& e)
        {
            callbacks.on_progress("Failed to load sound samples");
            file.clear();
            file.seekg(sound_start, std::ios::beg);
            log_file(activity, file, std::format("Failed to load sound samples {}", e.what()));
        }

        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        log_file(activity, file, "Generating meshes");
        generate_meshes(_mesh_data);
    }

    void Level::load_tr5_pc_remastered(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        _num_textiles = read_textiles_tr5_remastered(activity, file, callbacks);

        log_file(activity, file, "Reading Lara type");
        _lara_type = read<uint16_t>(file);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr5_pc_remastered_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = read_animations_tr4_5(activity, file, callbacks);
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_tr1_4(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);

        log_file(activity, file, "Skipping SPR marker");
        // Skip past the 'SPR' marker.
        file.seekg(3, std::ios::cur);

        _sprite_textures = read_sprite_textures(activity, file, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _flyby_cameras = read_flyby_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);

        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _animated_texture_uv_count = read_animated_textures_uv_count(activity, file, callbacks);

        log_file(activity, file, "Skipping TEX marker");
        file.seekg(3, std::ios::cur);

        _object_textures = read_object_textures_tr4_5(activity, file, callbacks);
        _ai_objects = read_ai_objects(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);

        load_sound_fx(activity, callbacks);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        log_file(activity, file, "Generating meshes");
        generate_meshes(_mesh_data);
    }
}
