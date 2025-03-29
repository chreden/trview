#include "Level.h"
#include "Level_common.h"
#include "Level_psx.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
#pragma pack(push, 1)
        struct tr4_psx_room_info
        {
            uint32_t        data_size;
            uint32_t        portal_size;
            uint32_t        sectors_size;
            uint32_t        light_size;
            uint32_t        static_mesh_size;
            tr_room_info    info;
            uint16_t        num_z_sectors;
            uint16_t        num_x_sectors;
            char            unknown_1[4];
            uint16_t        num_lights;
            uint16_t        num_meshes;
            char            unknown_2[1];
            uint8_t         alternate_group;
            char            unknown_3[22];
            int16_t         alternate_room;
            int16_t         flags;
        };
        static_assert(sizeof(tr4_psx_room_info) == 80);

        struct tr4_psx_level_info
        {
            int32_t  version;
            uint32_t sound_offsets;
            uint32_t sound_data_offset;
            uint32_t textiles_offset;
            uint32_t frames_offset;
            uint32_t room_data_offset;
            uint32_t models_offset;
            uint32_t unknown[2];
            uint32_t num_sounds;
            uint32_t sound_data_length;
            uint16_t clut_start;
            uint16_t num_rooms;
            char     unknown_2[2];
            uint16_t num_items;
            char     unknown_3[4];
            uint32_t room_data_size;
            uint32_t floor_data_size;
            uint32_t outside_room_size;
            uint32_t bounding_boxes_size;
            char     unknown_4[0x4];
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
            char     unknown_5[12];
            uint32_t unknown_offsets[7];
            uint32_t num_cameras;
            char     unknown_5a[4];
            int      camera_length;
            char     unknown_6[4];
            uint16_t num_ai_objects;
            char     unknown_7[38];
        };
        static_assert(sizeof(tr4_psx_level_info) == 228);

        struct tr4_psx_room_light
        {
            int32_t x;
            int32_t y;
            int32_t z;
            LightType type;
            uint8_t r;
            uint8_t g;
            uint8_t b;
            int16_t dx;
            int16_t dy;
            int16_t dz;
            uint16_t intensity;

            struct spot_light
            {
                uint8_t length;
                uint8_t cutoff;
                int16_t unused;
                int16_t in;
                int16_t out;
            };

            struct shadow_light
            {
                uint8_t hotspot;
                uint8_t falloff;
            };

            struct point_light
            {
                uint8_t hotspot;
                uint8_t falloff;
            };

            union
            {
                spot_light spot;
                shadow_light shadow;
                point_light point;
            };
        };
        static_assert(sizeof(tr4_psx_room_light) == 32);

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

        std::vector<tr3_room> read_rooms_tr4_psx(uint16_t num_rooms, std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks)
        {
            activity;
            callbacks;

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

                        const uint32_t data_start = static_cast<uint32_t>(file.tellg());

                        const uint32_t number_of_roomlets = read<uint32_t>(file);
                        const std::vector<uint32_t> roomlet_offsets = read_vector<uint32_t>(file, 16);

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

                        file.seekg(data_start + room_info.data_size, std::ios::beg);

                        const uint32_t portals_start = static_cast<uint32_t>(file.tellg());
                        room.portals = read_vector<tr_room_portal>(file, room_info.portal_size / sizeof(tr_room_portal));
                        file.seekg(portals_start + room_info.portal_size, std::ios::beg);

                        const uint32_t sectors_start = static_cast<uint32_t>(file.tellg());
                        room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
                        file.seekg(sectors_start + room_info.sectors_size, std::ios::beg);

                        const uint32_t lights_start = static_cast<uint32_t>(file.tellg());
                        room.lights = read_vector<tr4_psx_room_light>(file, room_info.num_lights)
                            | std::views::transform([](auto&& l)
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

                                }) | std::ranges::to<std::vector>();
                        file.seekg(lights_start + room_info.light_size, std::ios::beg);

                        const uint32_t statics_start = static_cast<uint32_t>(file.tellg());
                        room.static_meshes = read_vector<tr3_room_staticmesh>(file, room_info.num_meshes);
                        file.seekg(statics_start + room_info.static_mesh_size, std::ios::beg);

                        return room;
                    }) | std::ranges::to<std::vector>();
        }

        void read_sounds_tr4_psx(trview::Activity& activity, 
            std::basic_ispanstream<uint8_t>& file,
            const ILevel::LoadCallbacks& callbacks,
            uint32_t sounds_offset,
            uint32_t data_offset,
            uint32_t num_sounds,
            uint32_t data_length,
            uint32_t sample_frequency)
        {
            callbacks.on_progress("Reading sounds");
            log_file(activity, file, "Reading sounds");

            file.seekg(sounds_offset, std::ios::beg);
            const auto sound_offsets = read_vector<uint32_t>(file, num_sounds);

            file.seekg(data_offset, std::ios::beg);
            const auto sound_data = read_vector<byte>(file, data_length);

            for (uint32_t s = 0; s < sound_offsets.size(); ++s)
            {
                callbacks.on_progress(std::format("Loading sound {} of {}", s, sound_offsets.size()));
                log_file(activity, file, std::format("Loading sound {} of {}", s, sound_offsets.size()));
                const std::size_t offset = sound_offsets[s];
                const std::size_t size = s == sound_offsets.size() - 1 ?
                    sound_data.size() - offset - 1 :
                    sound_offsets[s + 1] - offset;
                callbacks.on_sound(static_cast<uint16_t>(s), convert_vag_to_wav({ &sound_data[offset], &sound_data[offset + size] }, sample_frequency));
            }

            log_file(activity, file, std::format("Read {} sounds", sound_offsets.size()));
        }
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

    void Level::load_tr4_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks;
        activity;

        file.seekg(0x7800, std::ios::beg);

        const uint32_t start = static_cast<uint32_t>(file.tellg());
        auto info = read<tr4_psx_level_info>(file);

        file.seekg(start + info.room_data_offset, std::ios::beg);
        _rooms = read_rooms_tr4_psx(info.num_rooms, file, activity, callbacks);

        _floor_data = read_vector<uint16_t>(file, info.floor_data_size / 2);

        // 'Room outside map':
        read_vector<uint16_t>(file, 729);
        skip(file, 2);
        // Outside room table:
        read_vector<uint8_t>(file, info.outside_room_size);
        // Bounding boxes
        skip(file, info.bounding_boxes_size);

        const uint32_t mesh_data_start = static_cast<uint32_t>(file.tellg());
        _mesh_data = read_vector<uint16_t>(file, info.mesh_data_size / sizeof(uint16_t));
        file.seekg(mesh_data_start + info.mesh_data_size, std::ios::beg);

        const uint32_t mesh_pointers_start = static_cast<uint32_t>(file.tellg());
        _mesh_pointers = read_vector<uint32_t>(file, info.mesh_pointer_size / sizeof(uint32_t));
        file.seekg(mesh_pointers_start + info.mesh_pointer_size, std::ios::beg);

        skip(file, info.animations_size);
        skip(file, info.state_changes_size);
        skip(file, info.dispatches_size);
        skip(file, info.commands_size);

        const uint32_t meshtree_pointers_start = static_cast<uint32_t>(file.tellg());
        _meshtree = read_vector<uint32_t>(file, info.meshtree_size / sizeof(uint32_t));
        file.seekg(meshtree_pointers_start + info.meshtree_size, std::ios::beg);

        skip(file, info.animated_texture_length);

        const uint32_t object_textures_start = static_cast<uint32_t>(file.tellg());
        _object_textures_psx = read_vector<tr_object_texture_psx>(file, info.texture_info_length / sizeof(tr_object_texture_psx));
        file.seekg(object_textures_start + info.texture_info_length, std::ios::beg);

        skip(file, info.sprite_info_length);
        const uint32_t room_textures_start = static_cast<uint32_t>(file.tellg());

        adjust_room_textures_psx();

        std::vector<tr_object_texture_psx> room_textures_psx;
        uint32_t num_room_textures = info.texture_info_length2 / sizeof(tr_object_texture_psx) / 3;
        for (auto i = 0u; i < num_room_textures; ++i)
        {
            room_textures_psx.push_back(read<tr_object_texture_psx>(file));
            skip(file, sizeof(tr_object_texture_psx) * 2);
        }

        _object_textures_psx.append_range(room_textures_psx);
        file.seekg(room_textures_start + info.texture_info_length2, std::ios::beg);
        skip(file, info.sfx_info_length);
        _sound_map = read_vector<int16_t>(file, 370);
        _sound_details = read_vector<tr_x_sound_details>(file, info.sample_info_length / sizeof(tr_x_sound_details));

        _entities = read_vector<tr4_psx_entity>(file, 256)
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

        _ai_objects = read_vector<tr4_psx_ai_object>(file, info.num_ai_objects)
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

        skip(file, info.unknown_offsets[0] + info.unknown_offsets[1]);
        skip(file, 2 * (info.unknown_offsets[2] + info.unknown_offsets[3] + info.unknown_offsets[4] + info.unknown_offsets[5] + info.unknown_offsets[6]));
        _cameras = read_vector<tr_camera>(file, info.num_cameras);

        file.seekg(start + info.frames_offset);
        _frames = read_vector<uint16_t>(file, info.frames_size / sizeof(uint16_t));

        file.seekg(start + info.models_offset);

        callbacks.on_progress("Reading models");
        log_file(activity, file, "Reading models");
        auto models = read_vector<tr4_psx_model>(file, 460);
        for (uint32_t m = 0; m < models.size(); ++m)
        {
            const auto& model = models[m];
            _models.push_back(tr_model
                {
                    .ID = m,
                    .NumMeshes = model.NumMeshes,
                    .StartingMesh = model.StartingMesh,
                    .MeshTree = model.MeshTree,
                    .FrameOffset = model.FrameOffset
                });
        }

        log_file(activity, file, std::format("Read {} models", _models.size()));
        skip(file, 320);

        auto static_meshes = read_vector<tr4_psx_staticmesh>(file, 70);
        for (uint32_t s = 0; s < static_meshes.size(); ++s)
        {
            const auto& staticmesh = static_meshes[s];
            _static_meshes.insert({ s, tr_staticmesh
                {
                    .ID = s,
                    .Mesh = staticmesh.Mesh,
                    .VisibilityBox = staticmesh.VisibilityBox,
                    .CollisionBox = staticmesh.CollisionBox,
                    .Flags = staticmesh.Flags
                } });
        }

        file.seekg(start + info.textiles_offset);

        // Create giant texture for whole memory:
        const auto textile_bytes = read_vector<uint8_t>(file, 0x80000);
        file.seekg(start + info.textiles_offset);

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

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        file.seekg(start + info.sound_offsets);
        read_sounds_tr4_psx(activity, file, callbacks, start + info.sound_offsets, start + info.sound_data_offset, info.num_sounds, info.sound_data_length, 11025);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }
}
