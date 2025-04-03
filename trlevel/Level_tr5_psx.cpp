#include "Level.h"
#include "Level_common.h"
#include "Level_psx.h"

#include <ranges>

namespace trlevel
{
    namespace
    {
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
    }

    void Level::load_tr5_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(313344);
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
        _sound_map = read_vector<int16_t>(file, 450);
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