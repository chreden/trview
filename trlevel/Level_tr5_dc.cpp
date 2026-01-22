#include "Level.h"
#include "Level_common.h"

#include <ranges>
#include <format>

namespace trlevel
{
    namespace
    {
        struct tr5_room_vertex_dreamcast
        {
            tr5_vertex vertex;
            tr5_vertex normal;
            uint32_t   colour;
            uint32_t   separator;
        };

        void load_tr5_dc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
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

            // Bit of extra padding for dreamcast:
            skip(file, 16);

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
                auto verts = convert_vertices(read_vector<tr5_room_vertex_dreamcast>(file, layer.num_vertices) |
                    std::views::transform([](auto&& v) -> tr5_room_vertex { return { .vertex = v.vertex, .normal = v.normal, .colour = v.colour }; }) |
                    std::ranges::to<std::vector>());
                std::copy(verts.begin(), verts.end(), std::back_inserter(room.data.vertices));
                ++layer_number;
            }

            file.seekg(room_end, std::ios::beg);
        }

        struct DreamcastPage
        {
        public:
            DreamcastPage(std::basic_ispanstream<uint8_t>& stream)
                : stream(stream), start(stream.tellg())
            {
            }

            ~DreamcastPage()
            {
                const std::size_t difference = static_cast<std::size_t>(stream.tellg()) - start;
                const std::size_t next = ((difference / 2048) + 1) * 2048;
                skip(stream, static_cast<uint32_t>(next - difference));
            }
        private:
            std::basic_ispanstream<uint8_t>& stream;
            std::size_t start;
        };

        struct DreamcastHeader
        {
            uint32_t textile_size{ 0u };
        };
    }

    void Level::load_tr5_dc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(0);

        DreamcastHeader header;
        {
            DreamcastPage page{ file };
            header = read<DreamcastHeader>(file);
        }

        // Special handling for JOBY1.
        if (header.textile_size == 3011032)
        {
            {
                DreamcastPage page{ file };
                uint32_t num_room_textiles = read<uint32_t>(file);
                uint32_t num_obj_textiles = read<uint32_t>(file);
                uint32_t num_bump_textiles = read<uint32_t>(file);
                log_file(activity, file, std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
                _num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;
            
                callbacks.on_progress(std::format("Reading {} 16-bit textiles", _num_textiles));
                log_file(activity, file, std::format("Reading {} 16-bit textiles", _num_textiles));

                for (uint32_t i = 0u; i < _num_textiles; ++i)
                {
                    uint32_t x = read<uint32_t>(file);
                    uint32_t tile_size = read<uint32_t>(file);
                    x; tile_size;
                    tr_textile16 textile;
                    file.read(reinterpret_cast<uint8_t*>(&textile), tile_size);
                    callbacks.on_textile(convert_textile(textile));
                }

                file.seekg(header.textile_size + 2048);
            }
        }
        else
        {
            {
                DreamcastPage page{ file };
                uint32_t num_room_textiles = read<uint32_t>(file);
                uint32_t num_obj_textiles = read<uint32_t>(file);
                uint32_t num_bump_textiles = read<uint32_t>(file);
                log_file(activity, file, std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
                _num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;
            }

            {
                DreamcastPage page{ file };
                callbacks.on_progress(std::format("Reading {} 16-bit textiles", _num_textiles));
                log_file(activity, file, std::format("Reading {} 16-bit textiles", _num_textiles));

                auto textile16 = read_vector<tr_textile16>(file, _num_textiles);
                for (const auto& textile : textile16)
                {
                    callbacks.on_textile(convert_textile(textile));
                }
                textile16 = {};
                file.seekg(header.textile_size + 2048);
            }
        }

        {
            DreamcastPage page{ file };
            log_file(activity, file, "Reading Lara type");
            _lara_type = read<uint16_t>(file);
            log_file(activity, file, std::format("Lara type: {}", _lara_type));
            log_file(activity, file, "Reading weather type");
            _weather_type = read<uint16_t>(file);
            log_file(activity, file, std::format("Weather type: {}", _weather_type));
            log_file(activity, file, "Skipping 28 unknown/padding bytes");
            file.seekg(28, std::ios::cur);
        }

        {
            DreamcastPage page{ file };
            read<uint32_t>(file);
            _rooms = read_rooms<uint32_t>(activity, file, callbacks, load_tr5_dc_room);
            _floor_data = read_floor_data(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
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
        }

        {
            DreamcastPage page{ file };
            log_file(activity, file, "Skipping SPR marker");
            // Skip past the 'SPR' marker.
            file.seekg(4, std::ios::cur);
            _sprite_textures = read_sprite_textures(activity, file, callbacks);
            _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            _cameras = read_cameras(activity, file, callbacks);
            _flyby_cameras = read_flyby_cameras(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            _sound_sources = read_sound_sources(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            const auto boxes = read_boxes(activity, file, callbacks);
            read_overlaps(activity, file, callbacks);
            read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        }

        {
            DreamcastPage page{ file };
            _animated_textures = read_animated_textures(activity, file, callbacks);
            _animated_texture_uv_count = read_animated_textures_uv_count(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            log_file(activity, file, "Skipping TEX marker");
            file.seekg(4, std::ios::cur);
            _object_textures = read_object_textures_tr5(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            _entities = read_entities(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            _ai_objects = read_ai_objects(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            read_demo_data(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            _sound_map = read_sound_map(activity, file, callbacks);
            _sound_details = read_sound_details(activity, file, callbacks);
            _sample_indices = read_sample_indices(activity, file, callbacks);
        }

        {
            DreamcastPage page{ file };
            skip(file, 8); // SAMP....
        }

        const auto sound_start = file.tellg();
        try
        {
            skip(file, 4); // TOSS
            read_sound_samples_tr4_5(file, activity, callbacks);
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
}
