#include "Level.h"
#include "Level_common.h"
#include "Level_psx.h"
#include "Level_tr3.h"
#include <trview.common/Algorithms.h>

#include <ranges>

namespace trlevel
{
    namespace
    {
#pragma pack(push, 1)
        struct tr4_psx_entity
        {
            char      unknown_0[12];
            int16_t   TypeID;
            char      unknown_1[10];
            int16_t   Room;
            char      unknown_2[14];
            uint16_t  Flags;
            char      unknown_3[2];
            int16_t   ocb;
            char      unknown_4[18];
            int32_t   x;
            int32_t   y;
            int32_t   z;
            char      unknown_5[2];
            int16_t   Angle;
            char      unknown_6[64];
        };
        static_assert(sizeof(tr4_psx_entity) == 144);

        struct tr4_psx_ai_object
        {
            uint16_t type_id;
            uint16_t room;
            int32_t  x;
            int32_t  y;
            int32_t  z;
            int16_t  ocb;
            uint16_t flags;
            int16_t  angle;
            uint16_t box;
        };
        static_assert(sizeof(tr4_psx_ai_object) == 24);

        struct tr4_psx_model
        {
            uint16_t NumMeshes;    // Number of meshes in this object
            uint16_t StartingMesh; // Starting mesh (offset into MeshPointers[])
            uint32_t MeshTree;     // Offset into MeshTree[]
            uint32_t FrameOffset;  // Byte offset into Frames[] (divide by 2 for Frames[i])
            char     unknown[52];
        };
        static_assert(sizeof(tr4_psx_model) == 64);

        struct tr4_psx_staticmesh
        {
            uint16_t Mesh; // Mesh (offset into MeshPointers[])
            uint16_t Flags;
            tr_bounding_box VisibilityBox;
            tr_bounding_box CollisionBox;
        };
        static_assert(sizeof(tr4_psx_staticmesh) == 28);

#pragma pack(pop)

        constexpr std::tuple<int, int> tile_to_x_y(uint16_t tile)
        {
            int x = ((tile & 0xf) * 64);
            int y = ((tile >> 4) & 0x1) * 256;
            x = (x - 512) * 2;
            return  { x, y };
        }

        constexpr std::tuple<uint16_t, uint16_t> clut_to_clut(uint16_t clut)
        {
            uint16_t x = (clut & 0x3f) * 16;
            uint16_t y = (clut >> 6) & 0x1ff;
            x = (x - 512) * 2;
            return { x, y };
        }

        std::vector<trview_room_vertex> convert_vertices_tr4_psx(std::vector<uint32_t> vertices, int32_t y_top)
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

        tr_x_room_light to_tr4_light(const tr4_psx_room_light& l)
        {
            tr_x_room_light new_light
            {
                .level_version = LevelVersion::Tomb4,
                .tr4 =
                {
                    .x = l.x,
                    .y = l.y,
                    .z = l.z,
                    .colour = {.Red = l.r, .Green = l.g, .Blue = l.b },
                    .light_type = l.type,
                    .intensity = l.intensity,
                    .dx = static_cast<float>(l.dx) / 4096.0f,
                    .dy = static_cast<float>(l.dy) / 4096.0f,
                    .dz = static_cast<float>(l.dz) / 4096.0f
                }
            };

            switch (l.type)
            {
            case LightType::Spot:
            {
                new_light.tr4.in = static_cast<float>(l.spot.in << 2) / 16384.0f;
                new_light.tr4.out = static_cast<float>(l.spot.out << 2) / 16384.0f;
                new_light.tr4.length = static_cast<float>(l.spot.length << 7);
                new_light.tr4.cutoff = static_cast<float>(l.spot.cutoff << 7);
                break;
            }
            case LightType::Shadow:
            {
                new_light.tr4.in = static_cast<float>(l.shadow.hotspot << 7);
                new_light.tr4.out = static_cast<float>(l.shadow.falloff << 7);
                break;
            }
            case LightType::Point:
            {
                new_light.tr4.in = static_cast<float>(l.point.hotspot << 7);
                new_light.tr4.out = static_cast<float>(l.point.falloff << 7);
                break;
            }
            }

            return new_light;
        }

        void load_tr4_psx_version_111_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);
            read_room_geometry_tr4_psx(file, room, NumDataWords * 2);

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_light_mode(activity, file, room);
            room.lights = read_vector<uint16_t, tr4_psx_room_light>(file)
                | std::views::transform(to_tr4_light)
                | std::ranges::to<std::vector>();
            read_room_static_meshes(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
            read_room_water_scheme(activity, file, room);
            read_room_reverb_info(activity, file, room);
            skip(file, 1); // filler in TR3
        }

        constexpr uint32_t model_count(PlatformAndVersion version)
        {
            switch (version.raw_version)
            {
            case -120:
                return 436;
            case -121:
                return 426;
            case -124:
                return 427;
            case -126:
                return 444;
            }
            return 465;
        }
    }

    std::vector<uint16_t> read_mesh_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading mesh data");
        log_file(activity, file, "Reading mesh data");
        const uint32_t mesh_data_start = static_cast<uint32_t>(file.tellg());
        const auto mesh_data = read_vector<uint16_t>(file, info.mesh_data_size / sizeof(uint16_t));
        file.seekg(mesh_data_start + info.mesh_data_size, std::ios::beg);
        log_file(activity, file, std::format("Read {} mesh data", mesh_data.size()));
        return mesh_data;
    }

    std::vector<uint32_t> read_mesh_pointers(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading mesh pointers");
        log_file(activity, file, "Reading mesh pointers");
        const uint32_t mesh_pointers_start = static_cast<uint32_t>(file.tellg());
        const auto mesh_pointers = read_vector<uint32_t>(file, info.mesh_pointer_size / sizeof(uint32_t));
        file.seekg(mesh_pointers_start + info.mesh_pointer_size, std::ios::beg);
        log_file(activity, file, std::format("Read {} mesh pointers", mesh_pointers.size()));
        return mesh_pointers;
    }

    std::vector<uint32_t> read_meshtree(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading mesh trees");
        log_file(activity, file, "Reading mesh trees");
        const uint32_t meshtree_pointers_start = static_cast<uint32_t>(file.tellg());
        const auto meshtree = read_vector<uint32_t>(file, info.meshtree_size / sizeof(uint32_t));
        file.seekg(meshtree_pointers_start + info.meshtree_size, std::ios::beg);
        log_file(activity, file, std::format("Read {} mesh trees", meshtree.size()));
        return meshtree;
    }

    std::vector<std::vector<int16_t>> read_animated_textures_tr4_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading animated textures");
        log_file(activity, file, "Reading animated textures");

        const auto start = file.tellg();
        std::vector<std::vector<int16_t>> textures;
        for (uint16_t index = 0; index < info.num_animated_textures; ++index)
        {
            int16_t num_texture_ids = read<int16_t>(file) + 1;
            textures.push_back(read_vector<int16_t>(file, num_texture_ids));
        }

        file.seekg(static_cast<std::size_t>(start) + info.animated_texture_length);
        return textures;
    }

    std::vector<tr_object_texture_psx> read_object_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading object textures");
        log_file(activity, file, "Reading object textures");
        const uint32_t object_textures_start = static_cast<uint32_t>(file.tellg());
        const auto object_textures = read_vector<tr_object_texture_psx>(file, info.texture_info_length / sizeof(tr_object_texture_psx));
        file.seekg(object_textures_start + info.texture_info_length, std::ios::beg);
        log_file(activity, file, std::format("Read {} object textures", object_textures.size()));
        return object_textures;
    }

    void read_room_geometry_tr4_psx(std::basic_ispanstream<uint8_t>& file, tr3_room& room, uint32_t data_size)
    {
        const uint32_t data_start = static_cast<uint32_t>(file.tellg());
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

            room.data.vertices.append_range(convert_vertices_tr4_psx(read_vector<uint32_t>(file, num_vertices), room.info.yTop));
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
        file.seekg(data_start + data_size, std::ios::beg);
    }

    std::vector<tr_object_texture_psx> read_room_textures(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading room textures");
        log_file(activity, file, "Reading room textures");
        const uint32_t room_textures_start = static_cast<uint32_t>(file.tellg());
        std::vector<tr_object_texture_psx> room_textures_psx;
        const uint32_t num_room_textures = info.texture_info_length2 / sizeof(tr_object_texture_psx) / 3;
        for (auto i = 0u; i < num_room_textures; ++i)
        {
            room_textures_psx.push_back(read<tr_object_texture_psx>(file));
            skip(file, sizeof(tr_object_texture_psx) * 2);
        }
        file.seekg(room_textures_start + info.texture_info_length2, std::ios::beg);
        log_file(activity, file, std::format("Read {} room textures", room_textures_psx.size()));
        return room_textures_psx;
    }

    std::vector<tr_sound_source> read_sound_sources(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading sound sources");
        log_file(activity, file, "Reading sound sources");
        const uint32_t sound_sources_start = static_cast<uint32_t>(file.tellg());
        const auto sound_sources = read_vector<tr_sound_source>(file, info.sfx_info_length / sizeof(tr_sound_source));
        file.seekg(sound_sources_start + info.sfx_info_length, std::ios::beg);
        log_file(activity, file, std::format("Read {} sound sources", sound_sources.size()));
        return sound_sources;
    }

    std::vector<tr2_entity> read_entities(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading entities");
        log_file(activity, file, "Reading entities");
        const auto entities = read_vector<tr4_psx_entity>(file, 256)
            | std::views::take(info.num_items)
            | std::views::transform([](auto&& e) -> tr2_entity
                {
                    return
                    {
                        .TypeID = e.TypeID,
                        .Room = e.Room,
                        .x = e.x,
                        .y = e.y,
                        .z = e.z,
                        .Angle = e.Angle,
                        .Intensity2 = e.ocb,
                        .Flags = e.Flags
                    };
                }) | std::ranges::to<std::vector>();
        log_file(activity, file, std::format("Read {} entities", entities.size()));
        return entities;
    }

    std::vector<tr4_ai_object> read_ai_objects(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading AI objects");
        log_file(activity, file, "Reading AI objects");
        const auto ai_objects = read_vector<tr4_psx_ai_object>(file, info.num_ai_objects)
            | std::views::transform([](auto&& a) -> tr4_ai_object
                {
                    return
                    {
                        .type_id = a.type_id,
                        .room = a.room,
                        .x = a.x,
                        .y = a.y,
                        .z = a.z,
                        .ocb = a.ocb,
                        .flags = a.flags,
                        .angle = a.angle
                    };
                }) | std::ranges::to<std::vector>();
        log_file(activity, file, std::format("Read {} AI objects", ai_objects.size()));
        return ai_objects;
    }

    std::vector<tr_model> read_models(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, uint32_t start, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks, uint32_t num_meshes)
    {
        file.seekg(start + info.models_offset);
        callbacks.on_progress("Reading models");
        log_file(activity, file, "Reading models");
        auto models = read_vector<tr4_psx_model>(file, num_meshes);
        std::vector<tr_model> converted_models;
        for (uint32_t m = 0; m < models.size(); ++m)
        {
            const auto& model = models[m];
            converted_models.push_back(tr_model
                {
                    .ID = m,
                    .NumMeshes = model.NumMeshes,
                    .StartingMesh = model.StartingMesh,
                    .MeshTree = model.MeshTree,
                    .FrameOffset = model.FrameOffset
                });
        }
        log_file(activity, file, std::format("Read {} models", converted_models.size()));
        return converted_models;
    }

    std::unordered_map<uint32_t, tr_staticmesh> read_static_meshes_tr4_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_statics)
    {
        callbacks.on_progress("Reading static meshes");
        log_file(activity, file, "Reading static meshes");
        auto static_meshes = read_vector<tr4_psx_staticmesh>(file, num_statics);
        std::unordered_map<uint32_t, tr_staticmesh> converted_static_meshes;
        for (uint32_t s = 0; s < static_meshes.size(); ++s)
        {
            const auto& staticmesh = static_meshes[s];
            converted_static_meshes.insert({ s, tr_staticmesh
                {
                    .ID = s,
                    .Mesh = staticmesh.Mesh,
                    .VisibilityBox = staticmesh.VisibilityBox,
                    .CollisionBox = staticmesh.CollisionBox,
                    .Flags = staticmesh.Flags
                } });
        }
        log_file(activity, file, std::format("Read {} static meshes", converted_static_meshes.size()));
        return converted_static_meshes;
    }

    std::vector<uint16_t> read_frames(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, uint32_t start, const tr4_psx_level_info& info, const ILevel::LoadCallbacks& callbacks)
    {
        file.seekg(start + info.frames_offset);
        callbacks.on_progress("Reading frames");
        log_file(activity, file, "Reading frames");
        const auto frames = read_vector<uint16_t>(file, info.frames_size / sizeof(uint16_t));
        log_file(activity, file, std::format("Read {} frames", frames.size()));
        return frames;
    }

    std::vector<tr3_room> read_rooms_tr4_psx(uint16_t num_rooms, std::basic_ispanstream<uint8_t>& file)
    {
        return read_vector<tr4_psx_room_info>(file, num_rooms)
            | std::views::transform([&](auto&& room_info)
                {
                    tr3_room room
                    {
                        .info = room_info.info,
                        .num_z_sectors = room_info.num_z_sectors,
                        .num_x_sectors = room_info.num_x_sectors,
                        .alternate_room = room_info.alternate_room,
                        .flags = room_info.flags,
                        .alternate_group = room_info.alternate_group,
                    };

                    read_room_geometry_tr4_psx(file, room, room_info.data_size);

                    const uint32_t portals_start = static_cast<uint32_t>(file.tellg());
                    room.portals = read_vector<tr_room_portal>(file, room_info.portal_size / sizeof(tr_room_portal));
                    file.seekg(portals_start + room_info.portal_size, std::ios::beg);

                    const uint32_t sectors_start = static_cast<uint32_t>(file.tellg());
                    room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
                    file.seekg(sectors_start + room_info.sectors_size, std::ios::beg);

                    const uint32_t lights_start = static_cast<uint32_t>(file.tellg());
                    room.lights = read_vector<tr4_psx_room_light>(file, room_info.num_lights)
                        | std::views::transform(to_tr4_light)
                        | std::ranges::to<std::vector>();
                    file.seekg(lights_start + room_info.light_size, std::ios::beg);

                    const uint32_t statics_start = static_cast<uint32_t>(file.tellg());
                    room.static_meshes = read_vector<tr3_room_staticmesh>(file, room_info.num_meshes);
                    file.seekg(statics_start + room_info.static_mesh_size, std::ios::beg);

                    return room;
                }) | std::ranges::to<std::vector>();
    }

    void Level::read_sounds_tr4_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks, uint32_t start, const tr4_psx_level_info& info, uint32_t sample_frequency)
    {
        callbacks.on_progress("Reading sounds");
        log_file(activity, file, "Reading sounds");
        file.seekg(start + info.sound_offsets, std::ios::beg);
        const auto sound_offsets = read_vector<uint32_t>(file, info.num_sounds);

        file.seekg(start + info.sound_data_offset, std::ios::beg);
        const auto sound_data = read_vector<byte>(file, info.sound_data_length);

        for (uint32_t s = 0; s < sound_offsets.size(); ++s)
        {
            callbacks.on_progress(std::format("Loading sound {} of {}", s, sound_offsets.size()));
            log_file(activity, file, std::format("Loading sound {} of {}", s, sound_offsets.size()));
            const std::size_t offset = sound_offsets[s];
            const std::size_t size = s == sound_offsets.size() - 1 ?
                sound_data.size() - offset - 1 :
                sound_offsets[s + 1] - offset;
            _sound_samples.push_back(convert_vag_to_wav({ &sound_data[offset], &sound_data[offset + size] }, sample_frequency));
        }

        log_file(activity, file, std::format("Read {} sounds", sound_offsets.size()));
    }

    void Level::generate_mesh_tr4_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
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
            // mesh.lights = read_vector<int16_t>(stream, vertices_count);
            // mesh.normals.push_back({ 0,0,0 });
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

        auto bad_rects = mesh.textured_rectangles | 
            std::views::filter([&](auto& r)
                {
                    return r.vertices[0] >= mesh.vertices.size() ||
                           r.vertices[1] >= mesh.vertices.size() ||
                           r.vertices[2] >= mesh.vertices.size() ||
                           r.vertices[3] >= mesh.vertices.size();
                }) | std::ranges::to<std::vector>();
        auto bad_tris = mesh.textured_triangles |
            std::views::filter([&](auto& t)
                {
                    return t.vertices[0] >= mesh.vertices.size() ||
                        t.vertices[1] >= mesh.vertices.size() ||
                        t.vertices[2] >= mesh.vertices.size();
                }) | std::ranges::to<std::vector>();

        if (!bad_rects.empty())
        {
            mesh.textured_rectangles.clear();
        }

        if (!bad_tris.empty())
        {
            mesh.textured_triangles.clear();
        }
    }

    void Level::generate_object_textures_tr4_psx(std::basic_ispanstream<uint8_t>& file, uint32_t start, const tr4_psx_level_info& info)
    {
        file.seekg(start + info.textiles_offset);
        const auto textile_bytes = read_vector<uint8_t>(file, 0x80000);

        auto convert_textile4_tr4_psx = [&](tr_object_texture_psx& t)
            {
                const auto [tx, ty] = tile_to_x_y(t.Tile);
                const auto [cx, cy] = clut_to_clut(t.Clut);
                tr_clut clut = *reinterpret_cast<const tr_clut*>(&textile_bytes[cy * 1024 + cx]);

                // Check if we've already converted this tile + clut
                for (auto i = _converted_t16.begin(); i < _converted_t16.end(); ++i)
                {
                    if (i->first == t.Tile && i->second == t.Clut)
                    {
                        t.Attribute = attribute_for_object_texture(t, clut),
                            t.Tile = static_cast<uint16_t>(std::distance(_converted_t16.begin(), i));
                        t.Clut = 0;
                        return;
                    }
                }
                // If not, create new conversion
                _converted_t16.push_back(std::make_pair(t.Tile, t.Clut));

                tr_textile16& tile16 = _textile16.emplace_back();
                for (int y = 0; y < 256; ++y)
                {
                    for (int x = 0; x < 256; ++x)
                    {
                        const std::size_t src_pixel = ((ty + y) * 1024) + (tx + (x / 2));
                        const std::size_t dest_pixel = y * 256 + x;
                        const tr_colorindex4 index = *reinterpret_cast<const tr_colorindex4*>(&textile_bytes[src_pixel]);
                        const tr_rgba5551& colour = clut.Colour[(x % 2) ? index.b : index.a];
                        tile16.Tile[dest_pixel] = (colour.Alpha << 15) | (colour.Red << 10) | (colour.Green << 5) | colour.Blue;
                    }
                }

                _num_textiles = static_cast<uint32_t>(_textile16.size());
                t.Attribute = attribute_for_object_texture(t, clut);
                t.Tile = static_cast<uint16_t>(_textile16.size() - 1);
                t.Clut = 0;
            };

        _object_textures = _object_textures_psx
            | std::views::transform([&](auto texture)
                {
                    convert_textile4_tr4_psx(texture);
                    return texture;
                })
            | std::views::transform([&](const auto texture) -> tr_object_texture
                {
                    return
                    {
                        .Attribute = texture.Attribute,
                        .TileAndFlag = texture.Tile,
                        .Vertices =
                        {
                            { 0, static_cast<uint8_t>(texture.x0), 0, texture.y0 },
                            { 0, static_cast<uint8_t>(texture.x1), 0, texture.y1 },
                            { 0, static_cast<uint8_t>(texture.x2), 0, texture.y2 },
                            { 0, static_cast<uint8_t>(texture.x3), 0, texture.y3 },
                        }
                    };
                })
            | std::ranges::to<std::vector>();
    }

    void Level::load_tr4_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        if (!is_supported_tr4_psx_version(peek<int32_t>(file)))
        {
            file.seekg(0x7000);
            const int32_t peeked_version_1 = peek<int32_t>(file);
            if (!is_supported_tr4_psx_version(peeked_version_1))
            {
                file.seekg(0x7800);
                const int32_t peeked_version_2 = peek<int32_t>(file);
                if (!is_supported_tr4_psx_version(peeked_version_2))
                {
                    file.seekg(0x6000);
                    const int32_t peeked_version_3 = peek<int32_t>(file);
                    if (!is_supported_tr4_psx_version(peeked_version_3))
                    {
                        return;
                    }
                }
            }
        }

        if (is_tr4_version_111(_platform_and_version))
        {
            return load_tr4_psx_version_111(file, activity, callbacks);
        }
        else if (is_tr4_version_120(_platform_and_version) || is_tr4_version_121(_platform_and_version))
        {
            return load_tr4_psx_opsm_90(file, activity, callbacks);
        }

        const uint32_t start = static_cast<uint32_t>(file.tellg());
        auto info = read<tr4_psx_level_info>(file);
        file.seekg(start + info.room_data_offset, std::ios::beg);
        _rooms = read_rooms_tr4_psx(info.num_rooms, file);
        _floor_data = read_vector<uint16_t>(file, info.floor_data_size / 2);

        // 'Room outside map':
        read_vector<uint16_t>(file, 729);
        skip(file, 2);
        // Outside room table:
        read_vector<uint8_t>(file, info.outside_room_size);
        // Bounding boxes
        skip(file, info.bounding_boxes_size);

        _mesh_data = read_mesh_data(activity, file, info, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, info, callbacks);
        skip(file, info.animations_size); // TODO: Load animations
        skip(file, info.state_changes_size);
        skip(file, info.dispatches_size);
        skip(file, info.commands_size);
        _meshtree = read_meshtree(activity, file, info, callbacks);
        _animated_textures = read_animated_textures_tr4_psx(activity, file, info, callbacks);;
        _animated_texture_uv_count = info.num_animated_uv_ranges;
        _object_textures_psx = read_object_textures(activity, file, info, callbacks);
        skip(file, info.sprite_info_length);
        adjust_room_textures_psx();
        _object_textures_psx.append_range(read_room_textures(activity, file, info, callbacks));
        _sound_sources = read_sound_sources(activity, file, info, callbacks);
        _sound_map = read_vector<int16_t>(file, 370);
        _sound_details = read_vector<tr_x_sound_details>(file, info.sample_info_length / sizeof(tr_x_sound_details));

        // Unsure about this hardcoded seek or if it even is related to demo data being present.
        // In the title level there is TR3 demo data after some unknown data.
        if (info.demo_data_length)
        {
            skip(file, 15872);
            skip(file, info.demo_data_length);
        }

        _entities = read_entities(activity, file, info, callbacks);
        _ai_objects = read_ai_objects(activity, file, info, callbacks);
        skip(file, info.boxes_length + info.overlaps_length);
        skip(file, 2 * (info.ground_zone_length + info.ground_zone_length2 + info.ground_zone_length3 + info.ground_zone_length4 + info.ground_zone_length5));
        _cameras = read_vector<tr_camera>(file, info.num_cameras);
        _flyby_cameras = read_vector<tr4_flyby_camera>(file, info.num_flyby_cameras);
        _frames = read_frames(activity, file, start, info, callbacks);
        _models = read_models(activity, file, start, info, callbacks, model_count(_platform_and_version));
        _static_meshes = read_static_meshes_tr4_psx(activity, file, callbacks, 60);
        generate_object_textures_tr4_psx(file, start, info);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        read_sounds_tr4_psx(file, activity, callbacks, start, info, 11025);
        generate_sounds(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    namespace
    {
        struct tr4_psx_level_info_opsm_90
        {
            int32_t version;
            uint8_t  unknown_0[2];
            uint16_t num_rooms;
            uint8_t  unknown_1[2];
            uint16_t num_items;
            uint16_t num_animated_textures;
            uint16_t num_animated_uv_ranges;
            uint32_t room_data_size;
            uint32_t floor_data_size;
            uint32_t outside_room_size;
            uint32_t bounding_boxes_size;
            uint8_t  unknown_4[0x4];
            uint32_t mesh_data_size;
            uint32_t mesh_pointer_size;
            uint32_t animations_size;
            uint32_t state_changes_size;
            uint32_t dispatches_size;
            uint32_t commands_size;
            uint32_t meshtree_size;
            uint32_t frames_size;
            uint32_t texture_info_length;
            uint32_t sprite_info_length;
            uint32_t texture_info_length2;
            uint32_t animated_texture_length;
            uint32_t sfx_info_length;
            uint32_t sample_info_length;
            uint32_t unknown_4a;
            uint32_t demo_data_length; // in title.psx
            uint32_t num_boxes;
            uint32_t boxes_length;
            uint32_t overlaps_length;
            uint32_t ground_zone_length;
            uint32_t ground_zone_length2;
            uint32_t ground_zone_length3;
            uint32_t ground_zone_length4;
            uint32_t ground_zone_length5;
            uint32_t num_cameras;
            uint32_t num_flyby_cameras;
            int      camera_length;
            uint32_t unknown_6;
            uint16_t num_ai_objects;
            char     unknown_7[38];
        };
    }

    void Level::load_tr4_psx_opsm_90(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(0);

        const uint32_t start = static_cast<uint32_t>(file.tellg());
        const auto opsm_info = read<tr4_psx_level_info_opsm_90>(file);
        tr4_psx_level_info info
        {
            .num_rooms = opsm_info.num_rooms,
            .num_items = opsm_info.num_items,
            .num_animated_textures = opsm_info.num_animated_textures,
            .num_animated_uv_ranges = opsm_info.num_animated_uv_ranges,
            .floor_data_size = opsm_info.floor_data_size,
            .outside_room_size = opsm_info.outside_room_size,
            .bounding_boxes_size = opsm_info.bounding_boxes_size,
            .mesh_data_size = opsm_info.mesh_data_size,
            .mesh_pointer_size = opsm_info.mesh_pointer_size,
            .animations_size = opsm_info.animations_size,
            .state_changes_size = opsm_info.state_changes_size,
            .dispatches_size = opsm_info.dispatches_size,
            .commands_size = opsm_info.commands_size,
            .meshtree_size = opsm_info.meshtree_size,
            .frames_size = opsm_info.frames_size,
            .texture_info_length = opsm_info.texture_info_length,
            .sprite_info_length = opsm_info.sprite_info_length,
            .texture_info_length2 = opsm_info.texture_info_length2,
            .animated_texture_length = opsm_info.animated_texture_length,
            .sfx_info_length = opsm_info.sfx_info_length,
            .sample_info_length = opsm_info.sample_info_length,
            .unknown_4a = opsm_info.unknown_4a,
            .demo_data_length = opsm_info.demo_data_length,
            .num_boxes = opsm_info.num_boxes,
            .boxes_length = opsm_info.boxes_length,
            .overlaps_length = opsm_info.overlaps_length,
            .ground_zone_length = opsm_info.ground_zone_length,
            .ground_zone_length2 = opsm_info.ground_zone_length2,
            .ground_zone_length3 = opsm_info.ground_zone_length3,
            .ground_zone_length4 = opsm_info.ground_zone_length4,
            .ground_zone_length5 = opsm_info.ground_zone_length5,
            .num_cameras = opsm_info.num_cameras,
            .num_flyby_cameras = opsm_info.num_flyby_cameras,
            .num_ai_objects = opsm_info.num_ai_objects
        };

        // Sounds without offsets
        info.num_sounds = read<uint32_t>(file);
        info.sound_offsets = static_cast<uint32_t>(file.tellg());
        info.sound_data_offset = static_cast<uint32_t>(file.tellg()) + info.num_sounds * sizeof(uint32_t) + 4;
        uint32_t sound_start = static_cast<uint32_t>(file.tellg());
        file.seekg(sound_start + info.num_sounds * sizeof(uint32_t));
        info.sound_data_length = read<uint32_t>(file);
        file.seekg(sound_start); 
        read_sounds_tr4_psx(file, activity, callbacks, start, info, 11025);

        info.textiles_offset = static_cast<uint32_t>(file.tellg());
        skip(file, 0x80000);

        _rooms = read_rooms_tr4_psx(info.num_rooms, file);
        _floor_data = read_vector<uint16_t>(file, info.floor_data_size / 2);

        // 'Room outside map':
        read_vector<uint16_t>(file, 729);
        skip(file, 2);
        // Outside room table:
        read_vector<uint8_t>(file, info.outside_room_size);
        // Bounding boxes
        skip(file, info.bounding_boxes_size);

        _mesh_data = read_mesh_data(activity, file, info, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, info, callbacks);

        skip(file, info.animations_size); // TODO: Read and store these
        skip(file, info.state_changes_size);
        skip(file, info.dispatches_size);
        skip(file, info.commands_size);
        _meshtree = read_meshtree(activity, file, info, callbacks);

        info.frames_offset = static_cast<uint32_t>(file.tellg());
        _frames = read_frames(activity, file, start, info, callbacks);

        _animated_textures = read_animated_textures_tr4_psx(activity, file, info, callbacks);
        _animated_texture_uv_count = info.num_animated_uv_ranges;

        _object_textures_psx = read_object_textures(activity, file, info, callbacks);
        skip(file, info.sprite_info_length);
        adjust_room_textures_psx();
        _object_textures_psx.append_range(read_room_textures(activity, file, info, callbacks));
        _sound_sources = read_sound_sources(activity, file, info, callbacks);

        _sound_map = read_vector<int16_t>(file, 370);
        _sound_details = read_vector<tr_x_sound_details>(file, info.sample_info_length / sizeof(tr_x_sound_details));

        // Unsure about this hardcoded seek or if it even is related to demo data being present.
        // In the title level there is TR3 demo data after some unknown data.
        if (info.demo_data_length)
        {
            skip(file, 12680);
            skip(file, info.demo_data_length);
        }

        _entities = read_entities(activity, file, info, callbacks);
        _ai_objects = read_ai_objects(activity, file, info, callbacks);
        skip(file, info.boxes_length + info.overlaps_length);
        skip(file, 2 * (info.ground_zone_length + info.ground_zone_length2 + info.ground_zone_length3 + info.ground_zone_length4 + info.ground_zone_length5));
        _cameras = read_vector<tr_camera>(file, info.num_cameras);
        _flyby_cameras = read_vector<tr4_flyby_camera>(file, info.num_flyby_cameras);
        info.models_offset = static_cast<uint32_t>(file.tellg());
        _models = read_models(activity, file, start, info, callbacks, model_count(_platform_and_version));
        _static_meshes = read_static_meshes_tr4_psx(activity, file, callbacks, 60);
        
        generate_object_textures_tr4_psx(file, start, info);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::load_tr4_psx_version_111(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        // For most of the checks and special handling in trview this
        // level may as well just be Tomb3.
        _platform_and_version.version = LevelVersion::Tomb3;

        int32_t version = read<int32_t>(file);
        version;
        read_sounds_psx(file, activity, callbacks, 11025);

        for (int i = 0; i < 13; ++i)
        {
            int size = read<int>(file);
            if (size != 0)
            {
                skip(file, size);
                int size2 = read<int>(file);
                skip(file, size2);
            }
        }

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr4_psx_version_111_room);
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
        read_animations_tr4_5(activity, file, callbacks);
        read_state_changes(activity, file, callbacks);
        read_anim_dispatches(activity, file, callbacks);
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

        // TODO: What this?
        skip(file, 4);

        const auto boxes = read_boxes(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);

        // Zones
        skip(file, static_cast<uint32_t>(boxes.size() * 20));

        skip(file, 2); // TODO: Figure out

        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities(activity, file, callbacks);
        read<int32_t>(file); // horizon colour
        read_room_textures_tr3_psx(file, activity, callbacks);
        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        generate_sounds(callbacks);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }
}
