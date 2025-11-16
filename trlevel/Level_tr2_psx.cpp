#include "Level.h"
#include "Level_common.h"
#include "Level_tr2.h"
#include "Level_psx.h"
#include "Level_tr1.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
#pragma pack(push, 1)
        struct tr_face3_psx
        {
            tr_face3 face;
            uint16_t reflective;
        };

        struct tr_face4_psx
        {
            tr_face4 face;
            uint16_t reflective;
        };

        struct tr2_face3_psx
        {
            uint16_t texture;
            uint16_t vertices[3];
        };

        struct tr2_face4_psx
        {
            uint16_t vertices[4];
        };
#pragma pack(pop)

        std::vector<trview_room_vertex> convert_vertices_tr2_psx(std::vector<uint32_t> vertices, int32_t y_top)
        {
            return vertices |
                std::views::transform([=](auto&& v)
                    {
                        const float colour = static_cast<float>(((v >> 15) & 0x7fff)) / 0x7fff;
                        return trview_room_vertex
                        {
                            .vertex =
                            {
                                .x = static_cast<int16_t>(((v >> 10) & 0x1f) << 10),
                                .y = static_cast<int16_t>((((v >> 5) & 0x1f) << 8) + y_top),
                                .z = static_cast<int16_t>((v & 0x1f) << 10)
                             },
                            .attributes = 0,
                            .colour = trview::Colour(1 - colour, 1 - colour, 1 - colour)
                        };
                    }) | std::ranges::to<std::vector>();
        }

        void load_tr2_psx_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);
            const auto at = file.tellg();
            if (NumDataWords > 0)
            {
                uint16_t num_vertices = read<uint16_t>(file);
                read<uint16_t>(file);
                room.data.vertices = convert_vertices_tr2_psx(read_vector<uint32_t>(file, num_vertices), room.info.yTop);

                const auto rectangle_start = file.tellg();
                uint16_t num_rectangles = read<uint16_t>(file);

                const auto rectangle_textures = read_vector<uint16_t>(file, num_rectangles);
                if ((file.tellg() - rectangle_start) % 4)
                {
                    skip(file, 2);
                }

                const auto rectangle_vertices = read_vector<tr2_face4_psx>(file, num_rectangles);
                for (auto i = 0u; i < num_rectangles; ++i)
                {
                    room.data.rectangles.push_back(
                        tr4_mesh_face4
                        {
                            .vertices =
                            {
                                static_cast<uint16_t>(rectangle_vertices[i].vertices[0] >> 2),
                                static_cast<uint16_t>(rectangle_vertices[i].vertices[1] >> 2),
                                static_cast<uint16_t>(rectangle_vertices[i].vertices[3] >> 2),
                                static_cast<uint16_t>(rectangle_vertices[i].vertices[2] >> 2)
                            },
                            .texture = rectangle_textures[i]
                        });
                }

                int count = read<int16_t>(file);
                if ((file.tellg() - rectangle_start) % 4)
                {
                    skip(file, 2);
                }
                const auto tris = read_vector<tr2_face3_psx>(file, count);
                room.data.triangles = tris |
                    std::views::transform([](auto&& t)
                        {
                            return tr4_mesh_face3
                            {
                                .vertices =
                                {
                                    static_cast<uint16_t>(t.vertices[0] >> 2),
                                    static_cast<uint16_t>(t.vertices[1] >> 2),
                                    static_cast<uint16_t>(t.vertices[2] >> 2)
                                },
                                .texture = t.texture,
                                .effects = 0
                            };
                        }) | std::ranges::to<std::vector>();
            }
            file.seekg(at, std::ios::beg);
            file.seekg(NumDataWords * 2, std::ios::cur);

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

        void load_tr2_psx_version_44_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);
            const auto at = file.tellg();
            if (NumDataWords > 0)
            {
                uint16_t num_vertices = read<uint16_t>(file);
                read<uint16_t>(file);
                room.data.vertices = convert_vertices_tr2_psx(read_vector<uint32_t>(file, num_vertices), room.info.yTop);

                const auto rectangles = read_vector<uint16_t, tr_face4>(file);
                room.data.rectangles = rectangles |
                    std::views::transform([](auto&& r)
                        {
                            return tr4_mesh_face4
                            {
                                .vertices =
                                {
                                    static_cast<uint16_t>(r.vertices[0] >> 2),
                                    static_cast<uint16_t>(r.vertices[1] >> 2),
                                    static_cast<uint16_t>(r.vertices[3] >> 2),
                                    static_cast<uint16_t>(r.vertices[2] >> 2)
                                },
                                .texture = r.texture,
                                .effects = 0
                            };
                        }) | std::ranges::to<std::vector>();

                const auto tris = read_vector<uint16_t, tr_face3>(file);
                room.data.triangles = tris |
                    std::views::transform([](auto&& t)
                        {
                            return tr4_mesh_face3
                            {
                                .vertices =
                                {
                                    static_cast<uint16_t>(t.vertices[0] >> 2),
                                    static_cast<uint16_t>(t.vertices[1] >> 2),
                                    static_cast<uint16_t>(t.vertices[2] >> 2)
                                },
                                .texture = t.texture,
                                .effects = 0
                            };
                        }) | std::ranges::to<std::vector>();
            }
            file.seekg(at, std::ios::beg);
            file.seekg(NumDataWords * 2, std::ios::cur);

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

        void load_tr2_psx_version_38_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            using namespace trlevel;

            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);
            const auto at = file.tellg();
            if (NumDataWords > 0)
            {
                uint16_t num_vertices = read<uint16_t>(file);
                num_vertices;
                read_room_vertices_tr1_psx(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);

                for (auto& rectangle : room.data.rectangles)
                {
                    std::swap(rectangle.vertices[2], rectangle.vertices[3]);
                }
            }
            file.seekg(at, std::ios::beg);
            file.seekg(NumDataWords * 2, std::ios::cur);

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
    }

    void Level::generate_mesh_tr2_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        mesh.centre = read<tr_vertex>(stream);
        mesh.coll_radius = read<int32_t>(stream);

        int16_t num_vertices = read<int16_t>(stream);
        mesh.vertices = convert_vertices(read_vector<tr_vertex_psx>(stream, abs(num_vertices)));
        if (num_vertices > 0)
        {
            mesh.normals = convert_vertices(read_vector<tr_vertex_psx>(stream, abs(num_vertices)));

            const auto reflective_rectangles = read_vector<int16_t, tr_face4_psx>(stream);
            const auto reflective_triangles = read_vector<int16_t, tr_face3_psx>(stream);

            mesh.textured_rectangles.append_range(reflective_rectangles
                | std::views::transform([](const auto& rect)
                    {
                        tr4_mesh_face4 new_face4;
                        new_face4.vertices[0] = rect.face.vertices[0] >> 3;
                        new_face4.vertices[1] = rect.face.vertices[1] >> 3;
                        new_face4.vertices[2] = rect.face.vertices[2] >> 3;
                        new_face4.vertices[3] = rect.face.vertices[3] >> 3;
                        new_face4.texture = rect.face.texture;
                        new_face4.effects = 0;
                        return new_face4;
                    }));
            mesh.textured_triangles.append_range(reflective_triangles
                | std::views::transform([](const auto& tri)
                    {
                        tr4_mesh_face3 new_face3;
                        new_face3.vertices[0] = tri.face.vertices[0] >> 3;
                        new_face3.vertices[1] = tri.face.vertices[1] >> 3;
                        new_face3.vertices[2] = tri.face.vertices[2] >> 3;
                        new_face3.texture = tri.face.texture;
                        new_face3.effects = 0;
                        return new_face3;
                    }));
        }
        else
        {
            mesh.lights = read_vector<int16_t>(stream, abs(num_vertices));
        }

        const auto rectangles = read_vector<int16_t, tr_face4>(stream);
        const auto triangles = read_vector<int16_t, tr_face3>(stream);

        mesh.textured_rectangles.append_range(rectangles
            | std::views::transform([](const auto& rect)
                {
                    tr4_mesh_face4 new_face4;
                    new_face4.vertices[0] = rect.vertices[0] >> 3;
                    new_face4.vertices[1] = rect.vertices[1] >> 3;
                    new_face4.vertices[2] = rect.vertices[2] >> 3;
                    new_face4.vertices[3] = rect.vertices[3] >> 3;
                    new_face4.texture = rect.texture;
                    new_face4.effects = 0;
                    return new_face4;
                }));
        mesh.textured_triangles.append_range(triangles
            | std::views::transform([](const auto& tri)
                {
                    tr4_mesh_face3 new_face3;
                    new_face3.vertices[0] = tri.vertices[0] >> 3;
                    new_face3.vertices[1] = tri.vertices[1] >> 3;
                    new_face3.vertices[2] = tri.vertices[2] >> 3;
                    new_face3.texture = tri.texture;
                    new_face3.effects = 0;
                    return new_face3;
                }));

    }

    void Level::generate_mesh_tr2_psx_version_44(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        const auto start = stream.tellg();

        mesh.centre = read<tr_vertex>(stream);
        mesh.coll_radius = read<int32_t>(stream);

        int16_t num_vertices = read<int16_t>(stream);
        mesh.vertices = convert_vertices(read_vector<tr_vertex_psx>(stream, abs(num_vertices)));
        if (num_vertices > 0)
        {
            mesh.normals = convert_vertices(read_vector<tr_vertex_psx>(stream, abs(num_vertices)));
        }
        else
        {
            mesh.lights = read_vector<int16_t>(stream, abs(num_vertices));
        }

        const auto rectangles = read_vector<int16_t, tr_face4>(stream);
        const auto triangles = read_vector<int16_t, tr_face3>(stream);

        mesh.textured_rectangles.append_range(rectangles
            | std::views::transform([](const auto& rect)
                {
                    tr4_mesh_face4 new_face4;
                    new_face4.vertices[0] = rect.vertices[0] >> 3;
                    new_face4.vertices[1] = rect.vertices[1] >> 3;
                    new_face4.vertices[2] = rect.vertices[2] >> 3;
                    new_face4.vertices[3] = rect.vertices[3] >> 3;
                    new_face4.texture = rect.texture;
                    new_face4.effects = 0;
                    return new_face4;
                }));
        mesh.textured_triangles.append_range(triangles
            | std::views::transform([](const auto& tri)
                {
                    tr4_mesh_face3 new_face3;
                    new_face3.vertices[0] = tri.vertices[0] >> 3;
                    new_face3.vertices[1] = tri.vertices[1] >> 3;
                    new_face3.vertices[2] = tri.vertices[2] >> 3;
                    new_face3.texture = tri.texture;
                    new_face3.effects = 0;
                    return new_face3;
                }));

        if ((stream.tellg() - start) % 4)
        {
            skip(stream, 2);
        }
    }

    void Level::generate_mesh_tr2_psx_version_38(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        const auto start = stream.tellg();

        mesh.centre = read<tr_vertex>(stream);
        mesh.coll_radius = read<int32_t>(stream);

        int16_t num_vertices = read<int16_t>(stream);
        mesh.vertices = convert_vertices(read_vector<tr_vertex_psx>(stream, abs(num_vertices)));
        if (num_vertices > 0)
        {
            mesh.normals = convert_vertices(read_vector<tr_vertex_psx>(stream, abs(num_vertices)));
        }
        else
        {
            mesh.lights = read_vector<int16_t>(stream, abs(num_vertices));
        }

        const auto rectangles = read_vector<int16_t, tr_face4>(stream);
        const auto triangles = read_vector<int16_t, tr_face3>(stream);

        mesh.textured_rectangles.append_range(rectangles
            | std::views::transform([](const auto& rect)
                {
                    tr4_mesh_face4 new_face4;
                    new_face4.vertices[0] = rect.vertices[0];
                    new_face4.vertices[1] = rect.vertices[1];
                    new_face4.vertices[2] = rect.vertices[2];
                    new_face4.vertices[3] = rect.vertices[3];
                    new_face4.texture = rect.texture;
                    new_face4.effects = 0;
                    return new_face4;
                }));
        mesh.textured_triangles.append_range(triangles
            | std::views::transform([](const auto& tri)
                {
                    tr4_mesh_face3 new_face3;
                    new_face3.vertices[0] = tri.vertices[0];
                    new_face3.vertices[1] = tri.vertices[1];
                    new_face3.vertices[2] = tri.vertices[2];
                    new_face3.texture = tri.texture;
                    new_face3.effects = 0;
                    return new_face3;
                }));

        if ((stream.tellg() - start) % 4)
        {
            skip(stream, 2);
        }
    }

    void Level::load_tr2_psx_version_44(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 16);
        uint32_t textile_address = read<uint32_t>(file);
        file.seekg(textile_address + 8, std::ios::beg);
        skip(file, 4);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr2_psx_version_44_room);
        _floor_data = read_floor_data(activity, file, callbacks);
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
        read_textiles_tr2_psx_version_44(file, activity, callbacks);
        read_object_textures_tr2_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read<int32_t>(file); // Unknown

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        };

        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);

        file.seekg(22);
        read_sounds_tr1_psx(file, activity, callbacks, 8000);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::load_tr2_psx_version_42(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 16);
        uint32_t textile_address = read<uint32_t>(file);
        file.seekg(textile_address + 8, std::ios::beg);

        read_textiles_tr2_psx_version_42(file, activity, callbacks);
        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr2_psx_version_44_room);
        _floor_data = read_floor_data(activity, file, callbacks);
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
        read_object_textures_tr2_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes_tr1(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);

        _entities = read_entities(activity, file, callbacks);
        read<int32_t>(file); // Unknown

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

    void Level::load_tr2_psx_version_38(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 16);
        uint32_t textile_address = read<uint32_t>(file);
        file.seekg(textile_address + 8, std::ios::beg);

        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");
        _num_textiles = 14;
        _textile4 = read_vector<tr_textile4>(file, _num_textiles);
        _clut = read_vector<tr_clut>(file, 1024);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));

        skip(file, 4);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr2_psx_version_38_room);
        _floor_data = read_floor_data(activity, file, callbacks);
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
        read_object_textures_tr2_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);
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
        read<int32_t>(file); // Unknown

        for (auto& t : _textile16)
        {
            for (uint16_t& pixel : t.Tile)
            {
                pixel |= ((pixel & 0x00ffffff) != 0) ? 0x8000 : 0x0000;
            }
            callbacks.on_textile(convert_textile(t));
        };

        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);

        file.seekg(22);
        read_sounds_tr1_psx(file, activity, callbacks, 11025);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
    }

    void Level::load_tr2_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        if (is_tr2_version_38(_platform_and_version))
        {
            return load_tr2_psx_version_38(file, activity, callbacks);
        }
        else if (is_tr2_version_42(_platform_and_version))
        {
            return load_tr2_psx_version_42(file, activity, callbacks);
        }
        else if (is_tr2_version_44(_platform_and_version))
        {
            return load_tr2_psx_version_44(file, activity, callbacks);
        }

        // TR2 has sound data at the start of the file so must seek back to the start.
        file.seekg(0, std::ios::beg);

        read_sounds_psx(file, activity, callbacks, 8000);
        read<uint32_t>(file); // version - already read.
        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr2_psx_room);
        _floor_data = read_floor_data(activity, file, callbacks);
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
        read_textiles_tr2_psx(file, activity, callbacks);
        skip(file, 4);
        read_object_textures_tr2_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);
        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read<int32_t>(file); // Unknown

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);

        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::read_object_textures_tr2_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
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
                    new_texture.Attribute = texture.Attribute;
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

    void Level::read_textiles_tr2_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
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
        _clut = read_vector<tr_clut>(file, num_cluts);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
    }

    void Level::read_textiles_tr2_psx_version_44(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");
        _num_textiles = read<uint32_t>(file);
        _textile4 = read_vector<tr_textile4>(file, _num_textiles);
        _clut = read_vector<tr_clut>(file, 2048);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
    }

    void Level::read_textiles_tr2_psx_version_42(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");
        _num_textiles = 18;
        _textile4 = read_vector<tr_textile4>(file, _num_textiles);
        _clut = read_vector<tr_clut>(file, 2048);
        skip(file, 4);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
    }
}
