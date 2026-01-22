#include "Level.h"
#include "Level_common.h"
#include "Level_tr3.h"
#include "Level_psx.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
        uint32_t get_skybox_id(PlatformAndVersion version)
        {
            if (is_tr3_ects(version))
            {
                return 312;
            }
            if (is_tr3_demo_55(version))
            {
                return 315;
            }
            return 355;
        }

        std::vector<trview_room_vertex> convert_vertices_tr3_psx(std::vector<uint32_t> vertices, int32_t y_top)
        {
            return vertices |
                std::views::transform([=](auto&& v)
                    {
                        const uint16_t colour = (v >> 15) & 0x7fff;
                        return trview_room_vertex
                        {
                            .vertex =
                            {
                                .x = static_cast<int16_t>(((v >> 10) & 0x1f) << 10),
                                .y = static_cast<int16_t>((((v >> 5) & 0x1f) << 8) + y_top),
                                .z = static_cast<int16_t>((v & 0x1f) << 10)
                             },
                            .attributes = 0,
                            .colour = trview::Colour(
                                static_cast<float>((colour & 0x1F)) / 0x1F,
                                static_cast<float>(((colour >> 5) & 0x1F)) / 0x1F,
                                static_cast<float>(((colour >> 10) & 0x1F)) / 0x1F)
                        };
                    }) | std::ranges::to<std::vector>();
        }

        void load_tr3_psx_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);

            const auto data_start = file.tellg();

            const uint32_t number_of_roomlets = read<uint32_t>(file);
            const std::vector<uint32_t> roomlet_offsets = read_vector<uint32_t>(file, number_of_roomlets);

            uint16_t total_vertices = 0;
            uint32_t previous_offset = 0;
            for (std::size_t i = 0; i < number_of_roomlets; ++i)
            {
                previous_offset = roomlet_offsets[i];

                file.seekg(static_cast<std::size_t>(data_start) + roomlet_offsets[i]);

                const auto bounding_box = read_vector<uint8_t>(file, 6);
                bounding_box;

                uint8_t num_vertices = read<uint8_t>(file);
                uint8_t num_triangles = read<uint8_t>(file);

                room.data.vertices.append_range(convert_vertices_tr3_psx(read_vector<uint32_t>(file, num_vertices), room.info.yTop));
                room.data.triangles.append_range(convert_tr3_psx_room_triangles(read_vector<uint32_t>(file, num_triangles), total_vertices));

                bool more_quads = true;
                while (more_quads)
                {
                    const uint32_t tex_info = read<uint32_t>(file);

                    uint32_t face_index = 0;
                    for (uint32_t face : read_vector<uint32_t>(file, 3))
                    {
                        if (((tex_info >> (10 * face_index)) & 0x3ff) == 0x3ff)
                        {
                            more_quads = false;
                            break;
                        }

                        room.data.rectangles.push_back
                        (
                            tr4_mesh_face4
                            {
                                .vertices =
                                {
                                    static_cast<uint16_t>(total_vertices + (face & 0x7f)),
                                    static_cast<uint16_t>(total_vertices + ((face >> 7) & 0x7f)),
                                    static_cast<uint16_t>(total_vertices + ((face >> 21) & 0x7f)),    // swapped with last
                                    static_cast<uint16_t>(total_vertices + ((face >> 14) & 0x7f))
                                },
                                .texture = static_cast<uint16_t>((tex_info >> (face_index * 10)) & 0x3ff),
                                .effects = 0
                            }
                        );
                        face_index++;
                    }
                }

                total_vertices += num_vertices;
            }

            file.seekg(data_start, std::ios::beg);
            skip(file, NumDataWords * 2);

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_light_mode(activity, file, room);
            read_room_lights_tr3(activity, file, room);
            read_room_static_meshes(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
            read_room_water_scheme(activity, file, room);
            read_room_reverb_info(activity, file, room);
            skip(file, 1); // filler in TR3
        }
    }

    void Level::generate_mesh_tr3_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        const uint32_t skybox_id = get_skybox_id(_platform_and_version);
        const auto skybox_model = std::ranges::find_if(_models, [skybox_id](auto&& m) { return m.ID == skybox_id; });
        if (skybox_model != _models.end())
        {
            if (stream.tellg() == _mesh_pointers[skybox_model->StartingMesh])
            {
                return;
            }
        }

        mesh.centre = read<tr_vertex>(stream);
        mesh.coll_radius = read<int16_t>(stream);
        uint8_t vertices_count = read<uint8_t>(stream);
        uint8_t flags = read<uint8_t>(stream);
        uint16_t face_data_offset = read<uint16_t>(stream);
        const auto at = stream.tellg();

        mesh.vertices = convert_vertices(read_vector<tr_vertex_psx>(stream, vertices_count));
        if ((flags & 0x80) == 0)
        {
            mesh.normals = convert_vertices(read_vector<tr_vertex_psx>(stream, vertices_count));
        }
        else
        {
            mesh.lights = read_vector<int16_t>(stream, vertices_count);
            mesh.normals.push_back({ 0,0,0 });
        }

        stream.seekg(static_cast<std::size_t>(at) + face_data_offset, std::ios::beg);

        uint16_t num_triangles = read<uint16_t>(stream);
        uint16_t num_rectangles = read<uint16_t>(stream);

        if (num_triangles)
        {
            const auto triangle_start = stream.tellg();
            const uint16_t* data = reinterpret_cast<const uint16_t*>(stream.span().data() + stream.tellg());
            const uint16_t* ptr = data;
            const auto start = ptr;

            uint32_t face_a = 0;
            uint32_t face_b = 0;
            for (int t = 0; t < num_triangles; ++t)
            {
                if (!(t % 4))
                {
                    face_a = *ptr++;
                    face_a |= (*ptr++) << 16;
                }

                face_b = *ptr++;
                face_b |= (*ptr++) << 16;

                tr4_mesh_face3 tri
                {
                    .vertices = { face_b & 0xff, (face_b >> 8) & 0xff, (face_b >> 16) & 0xff },
                    .texture = (face_a & 0xff) | (((face_b >> 24) & 0xff) << 8)
                };
                mesh.textured_triangles.push_back(tri);

                face_a >>= 8;
            }

            const auto skip_amount = ptr - start;
            skip(stream, static_cast<uint32_t>(skip_amount * 2));
        }

        if (num_rectangles)
        {
            const uint16_t* data = reinterpret_cast<const uint16_t*>(stream.span().data() + stream.tellg());
            const uint16_t* ptr = data;
            const auto start = ptr;

            uint32_t face_a = 0;
            uint32_t face_b = 0;
            for (int r = 0; r < num_rectangles; ++r)
            {
                if (!(r % 2))
                {
                    face_a = *ptr++;
                    face_a |= (*ptr++) << 16;
                }

                face_b = *ptr++;
                face_b |= (*ptr++) << 16;

                tr4_mesh_face4 rect
                {
                    .vertices = { face_b & 0xff, (face_b >> 8) & 0xff, (face_b >> 24) & 0xff, (face_b >> 16) & 0xff },
                    .texture = (face_a & 0xffff)
                };
                mesh.textured_rectangles.push_back(rect);

                face_a >>= 16;
            }

            const auto skip_amount = ptr - start;
            skip(stream, static_cast<uint32_t>(skip_amount * 2));
        }
    }

    void Level::load_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        read_sounds_psx(file, activity, callbacks, 11025);
        const bool ects = is_tr3_ects(_platform_and_version);

        for (int i = 0; i < (ects ? 10 : 13); ++i)
        {
            int size = read<int>(file);
            if (size != 0)
            {
                skip(file, size);
                int size2 = read<int>(file);
                skip(file, size2);
            }
            else if (ects)
            {
                skip(file, 4);
            }
        }

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr3_psx_room);
        _floor_data = read_floor_data(activity, file, callbacks);

        // 'Room outside map':
        read_vector<uint16_t>(file, 729);
        // Outside room table:
        read_vector<uint32_t, uint8_t>(file);
        // Bounding boxes
        int bb_count = read<int>(file);
        skip(file, bb_count * 8);

        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);
        _animations = convert_animations(read_animations_tr1_3(activity, file, callbacks));
        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_psx(activity, file, callbacks);
        _static_meshes = read_static_meshes(activity, file, callbacks);
        read_textiles_tr3_psx(file, activity, callbacks);
        read_object_textures_tr3_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read<int32_t>(file); // horizon colour
        read_room_textures_tr3_psx(file, activity, callbacks);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        };

        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::read_object_textures_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading object textures");
        log_file(activity, file, "Reading object textures");
        _object_textures_psx = read_vector<uint32_t, tr_object_texture_psx>(file);
        _object_textures = _object_textures_psx
            | std::views::transform([&](const auto texture)
                {
                    tr_object_texture_psx new_texture = texture;
                    new_texture.Tile = convert_textile4(texture.Tile, texture.Clut);
                    new_texture.Clut = 0U; // Unneeded after conversion
                    new_texture.Attribute = attribute_for_object_texture(texture, _clut[texture.Clut]);
                    return new_texture;
                })
            | std::views::transform([&](const auto texture) -> tr_object_texture
                {
                    return
                    {
                        .Attribute = texture.Attribute,
                        .TileAndFlag = texture.Tile,
                        .Vertices =
                        {
                            { 0, texture.x0, 0, texture.y0 },
                            { 0, texture.x1, 0, texture.y1 },
                            { 0, texture.x2, 0, texture.y2 },
                            { 0, texture.x3, 0, texture.y3 },
                        }
                    };
                })
            | std::ranges::to<std::vector>();
        log_file(activity, file, std::format("Read {} object textures", _object_textures.size()));
    }

    void Level::read_room_textures_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading room textures");
        log_file(activity, file, "Reading room textures");

        std::vector<tr_object_texture_psx> room_textures_psx;
        uint32_t num_room_textures = read<uint32_t>(file);
        for (auto i = 0u; i < num_room_textures; ++i)
        {
            room_textures_psx.push_back(read<tr_object_texture_psx>(file));
            skip(file, sizeof(tr_object_texture_psx) * 2);
        }

        auto room_textures_object_psx = room_textures_psx
            | std::views::transform([&](const auto texture)
                {
                    tr_object_texture_psx new_texture = texture;
                    new_texture.Tile = convert_textile4(texture.Tile, texture.Clut);
                    new_texture.Clut = 0U; // Unneeded after conversion
                    new_texture.Attribute = attribute_for_object_texture(texture, _clut[texture.Clut]);
                    return new_texture;
                })
            | std::ranges::to<std::vector>();
        auto room_textures = room_textures_object_psx
            | std::views::transform([&](const auto texture) -> tr_object_texture
                {
                    return
                    {
                        .Attribute = texture.Attribute,
                        .TileAndFlag = texture.Tile,
                        .Vertices =
                        {
                            { 0, texture.x0, 0, texture.y0 },
                            { 0, texture.x1, 0, texture.y1 },
                            { 0, texture.x2, 0, texture.y2 },
                            { 0, texture.x3, 0, texture.y3 },
                        }
                    };
                })
            | std::ranges::to<std::vector>();
        log_file(activity, file, std::format("Read {} room textures", room_textures.size()));

        adjust_room_textures_psx();
        _object_textures_psx.append_range(room_textures_object_psx);
        _object_textures.append_range(room_textures);
    }

    void Level::read_textiles_tr3_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");
        _num_textiles = read<uint32_t>(file);
        _textile4 = read_vector<tr_textile4>(file, _num_textiles);

        uint32_t num_cluts = read<uint32_t>(file);
        if (num_cluts & 0xffff0000)
        {
            file.seekg(-2, std::ios::cur);
            num_cluts = read<uint32_t>(file);
        }
        _clut = read_vector<tr_clut>(file, num_cluts * 2);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
    }
}
