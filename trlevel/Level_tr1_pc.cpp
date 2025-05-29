#include "Level.h"
#include "Level_common.h"
#include "Level_tr1.h"
#include "LevelLoadException.h"

#include <filesystem>
#include <ranges>
#include <span>

namespace trlevel
{
    namespace
    {
#pragma pack(push, 1)
        struct tr_staticmesh_may_1996   // 18 bytes
        {
            uint32_t ID;   // Static Mesh Identifier
            uint16_t Mesh; // Mesh (offset into MeshPointers[])
            tr_bounding_box VisibilityBox;
        };

        struct tr_entity_may_1996
        {
            int16_t TypeID;
            int16_t Room;
            int32_t x;
            int32_t y;
            int32_t z;
            int16_t Angle;
            uint16_t Flags;
        };

        struct tr_object_texture_may_1996
        {
            uint8_t x;
            uint8_t y;
            uint16_t Tile;
            tr_object_texture_vert size;
        };

        struct tr_sprite_texture_may_1996   // 16 bytes
        {
            uint8_t x;
            uint8_t y;
            uint16_t Tile;
            uint16_t Width;        // (ActualWidth  * 256) + 255
            uint16_t Height;       // (ActualHeight * 256) + 255
            int16_t LeftSide;
            int16_t TopSide;
            int16_t RightSide;
            int16_t BottomSide;
        };

#pragma pack(pop)

        enum class PrimitiveType : uint16_t
        {
            Triangle2 = 2,
            Rectangle3 = 3,
            ColouredTriangle = 4,
            ColouredRectangle = 5,
            TexturedTriangle = 8,
            TexturedRectangle = 9,
            Triangle10 = 10,
            Rectangle11 = 11,
            TransparentTexturedTriangle = 16,
            TransparentTexturedRectangle = 17
        };

        enum class RoomPrimitive : uint16_t
        {
            TransparentTriangle = 32,
            TransparentRectangle,
            InvisibleTriangle,
            InvisibleRectangle,
            TexturedTriangle,
            TexturedRectangle,
            Sprite = 39
        };

        void read_room_lights_tr1_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading lights");
            room.lights = convert_lights(read_vector<uint16_t, tr_room_light>(file));
            log_file(activity, file, std::format("Read {} lights", room.lights.size()));
        }

        void read_room_static_meshes_tr1_pc(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading static meshes");
            room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh>(file));
            log_file(activity, file, std::format("Read {} static meshes", room.static_meshes.size()));
        }

        uint16_t attribute_for_object_texture(tr_object_texture& ot, const std::vector<tr_textile8>& textiles)
        {
            const auto [min_x, max_x] = std::ranges::minmax(ot.Vertices | std::views::transform([](auto&& v) { return v.x_whole; }));
            const auto [min_y, max_y] = std::ranges::minmax(ot.Vertices | std::views::transform([](auto&& v) { return v.y_whole; }));
            for (uint8_t y = min_y; y < max_y; ++y)
            {
                for (uint8_t x = min_x; x < max_x; ++x)
                {
                    if (textiles[ot.TileAndFlag].Tile[x + y * 256] == 0)
                    {
                        return 1;
                    }
                }
            }
            return 0;
        }

        void read_room_mesh_pre_21(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            uint32_t NumDataWords = read_num_data_words(activity, file);
            auto at = file.tellg();

            // Read actual room data.
            if (NumDataWords > 0)
            {
                read_room_vertices_tr1(activity, file, room);

                uint16_t num_primitives = read<uint16_t>(file);
                for (uint16_t i = 0; i < num_primitives; ++i)
                {
                    RoomPrimitive value = read<RoomPrimitive>(file);
                    switch (value)
                    {
                    case RoomPrimitive::InvisibleTriangle:
                    {
                        skip(file, sizeof(tr_face3));
                        break;
                    }
                    case RoomPrimitive::InvisibleRectangle:
                    {
                        skip(file, sizeof(tr_face4));
                        break;
                    }
                    case RoomPrimitive::TexturedTriangle:
                    {
                        tr_face3 tri = read<tr_face3>(file);
                        tr4_mesh_face3 new_face3;
                        memcpy(new_face3.vertices, tri.vertices, sizeof(tri.vertices));
                        new_face3.texture = tri.texture;
                        new_face3.effects = 0;
                        room.data.triangles.push_back(new_face3);
                        break;
                    }
                    case RoomPrimitive::TransparentRectangle:
                    case RoomPrimitive::TexturedRectangle:
                    {
                        tr_face4 rect = read<tr_face4>(file);
                        tr4_mesh_face4 new_face4;
                        memcpy(new_face4.vertices, rect.vertices, sizeof(rect.vertices));
                        new_face4.texture = rect.texture;
                        new_face4.effects = 0;
                        room.data.rectangles.push_back(new_face4);
                        break;
                    }
                    case RoomPrimitive::Sprite:
                    {
                        room.data.sprites.push_back(read<tr_room_sprite>(file));
                        break;
                    }
                    }
                }
            }

            file.seekg(at, std::ios::beg);
            skip(file, NumDataWords * 2);

            auto [min, max] = std::ranges::minmax(room.data.vertices | std::views::transform([](auto&& v) { return v.vertex.y; }));
            room.info.yTop = min;
            room.info.yBottom = max;
        }

        void load_tr1_pc_room_version_21(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            read_room_mesh_pre_21(activity, file, room);
            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_lights_tr1_pc(activity, file, room);
            read_room_static_meshes_tr1_pc(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
        }

        void load_tr1_pc_room_may_1996(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info.x = read<int32_t>(file);
            room.info.z = read<int32_t>(file);
            read_room_mesh_pre_21(activity, file, room);
            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_lights_tr1_pc(activity, file, room);
            read_room_static_meshes_tr1_pc(activity, file, room);
            read_room_flags(activity, file, room);
        }

        void load_tr1_pc_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);

            // Read actual room data.
            if (NumDataWords > 0)
            {
                read_room_vertices_tr1(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);
                read_room_sprites(activity, file, room);
            }

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_lights_tr1_pc(activity, file, room);
            read_room_static_meshes_tr1_pc(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
        }

        tr_object_texture convert_object_texture(const tr_object_texture_may_1996& texture)
        {
            return
            {
                .Attribute = 0,
                .TileAndFlag = texture.Tile,
                .Vertices =
                {
                    { 0, texture.x, 0, texture.y },
                    { texture.size.x_frac, static_cast<uint8_t>(texture.x + texture.size.x_whole), 0, texture.y },
                    { 0, texture.x, texture.size.y_frac, static_cast<uint8_t>(texture.y + texture.size.y_whole) },
                    { texture.size.x_frac, static_cast<uint8_t>(texture.x + texture.size.x_whole), texture.size.y_frac, static_cast<uint8_t>(texture.y + texture.size.y_whole) }
                }
            };
        }

        std::vector<std::vector<int16_t>> read_animated_textures_21(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t room_texture_adjustment)
        {
            callbacks.on_progress("Reading animated textures");
            log_file(activity, file, "Reading animated textures");

            uint32_t num_animated_texture_sequences = read<uint32_t>(file);

            std::vector<std::vector<int16_t>> textures;
            for (uint32_t t = 0; t < num_animated_texture_sequences; ++t)
            {
                int16_t start = static_cast<int16_t>(read<uint32_t>(file));
                int16_t end = static_cast<int16_t>(read<uint32_t>(file));
                textures.push_back({ std::from_range, std::views::iota(static_cast<int16_t>(start + room_texture_adjustment), static_cast<int16_t>(end + 1 + room_texture_adjustment)) });
            }

            log_file(activity, file, std::format("Read {} animated textures sequences", textures.size()));
            return textures;
        }
    }

    std::vector<tr_box> read_boxes_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading boxes");
        log_file(activity, file, "Reading boxes");
        const auto boxes = read_vector<uint32_t, tr_box>(file);
        log_file(activity, file, std::format("Read {} boxes", static_cast<uint32_t>(boxes.size())));
        return boxes;
    }

    std::vector<tr2_entity> read_entities_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading entities");
        log_file(activity, file, "Reading entities");
        auto entities = convert_entities(read_vector<uint32_t, tr_entity>(file));
        log_file(activity, file, std::format("Read {} entities", entities.size()));
        return entities;
    }

    void read_room_vertices_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading vertices");
        room.data.vertices = convert_vertices(read_vector<int16_t, tr_room_vertex>(file));
        log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));
    }

    void read_zones_tr1(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes)
    {
        callbacks.on_progress("Reading zones");
        log_file(activity, file, "Reading zones");
        std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 6);
        log_file(activity, file, std::format("Read {} zones", zones.size()));
    }

    void Level::generate_sound_samples(const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Generating sound samples");
        for (int s = 0; s < _sample_indices.size(); ++s)
        {
            const auto start = _sample_indices[s];
            const auto end = s + 1 < _sample_indices.size() ? _sample_indices[s + 1] : _sound_data.size();
            _sound_samples.push_back({ _sound_data.begin() + start, _sound_data.begin() + end });
        }
    }

    void Level::generate_mesh_tr1_pc_version_21(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        mesh.centre = read<tr_vertex>(stream);
        mesh.coll_radius = read<int32_t>(stream);
        int16_t vertices_count = read<int16_t>(stream);
        vertices_count = static_cast<int16_t>(std::abs(vertices_count));

        mesh.vertices = read_vector<tr_vertex>(stream, vertices_count);
        int16_t normals_count = read<int16_t>(stream);

        for (int i = 0; i < vertices_count; ++i)
        {
            if (normals_count > 0)
            {
                mesh.normals.push_back(read<tr_vertex>(stream));
            }
            else
            {
                mesh.lights.push_back(read<int16_t>(stream)); // intensity
                mesh.normals.push_back({ 0, 0, 0 });
            }
        }

        std::vector<tr_face3> coloured_triangles;
        std::vector<tr_face4> coloured_rectangles;
        std::vector<tr_face3> textured_triangles;
        std::vector<tr_face4> textured_rectangles;

        const uint16_t num_primitives = read<uint16_t>(stream);
        for (uint16_t i = 0; i < num_primitives; ++i)
        {
            PrimitiveType primitive_type = read<PrimitiveType>(stream);
            switch (primitive_type)
            {
            case PrimitiveType::ColouredTriangle:
            {
                coloured_triangles.push_back(read<tr_face3>(stream));
                break;
            }
            case PrimitiveType::ColouredRectangle:
            {
                coloured_rectangles.push_back(read<tr_face4>(stream));
                break;
            }
            case PrimitiveType::Triangle2:
            case PrimitiveType::TexturedTriangle:
            case PrimitiveType::Triangle10:
            case PrimitiveType::TransparentTexturedTriangle:
            {
                textured_triangles.push_back(read<tr_face3>(stream));
                break;
            }
            case PrimitiveType::Rectangle3:
            case PrimitiveType::TexturedRectangle:
            case PrimitiveType::Rectangle11:
            case PrimitiveType::TransparentTexturedRectangle:
            {
                textured_rectangles.push_back(read<tr_face4>(stream));
                break;
            }
            }
        }

        mesh.textured_rectangles = convert_rectangles(textured_rectangles);
        mesh.textured_triangles = convert_triangles(textured_triangles);
        mesh.coloured_rectangles = coloured_rectangles;
        mesh.coloured_triangles = coloured_triangles;
    }

    void Level::generate_mesh_tr1_pc_may_1996(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        mesh.centre = { .x = 0, .y = 0, .z = 0 };
        mesh.coll_radius = read<uint16_t>(stream);
        int16_t vertices_count = read<int16_t>(stream);
        vertices_count = static_cast<int16_t>(std::abs(vertices_count));

        mesh.vertices = read_vector<tr_vertex>(stream, vertices_count);
        int16_t normals_count = read<int16_t>(stream);

        for (int i = 0; i < vertices_count; ++i)
        {
            if (normals_count > 0)
            {
                mesh.normals.push_back(read<tr_vertex>(stream));
            }
            else
            {
                mesh.lights.push_back(read<int16_t>(stream)); // intensity
                mesh.normals.push_back({ 0, 0, 0 });
            }
        }

        std::vector<tr_face3> coloured_triangles;
        std::vector<tr_face4> coloured_rectangles;
        std::vector<tr_face3> textured_triangles;
        std::vector<tr_face4> textured_rectangles;

        const uint16_t num_primitives = read<uint16_t>(stream);
        for (uint16_t i = 0; i < num_primitives; ++i)
        {
            PrimitiveType primitive_type = read<PrimitiveType>(stream);
            switch (primitive_type)
            {
                case PrimitiveType::ColouredTriangle:
                {
                    coloured_triangles.push_back(read<tr_face3>(stream));
                    break;
                }
                case PrimitiveType::ColouredRectangle:
                {
                    coloured_rectangles.push_back(read<tr_face4>(stream));
                    break;
                }
                case PrimitiveType::Triangle2:
                case PrimitiveType::TexturedTriangle:
                case PrimitiveType::Triangle10:
                case PrimitiveType::TransparentTexturedTriangle:
                {
                    textured_triangles.push_back(read<tr_face3>(stream));
                    break;
                }
                case PrimitiveType::Rectangle3:
                case PrimitiveType::TexturedRectangle:
                case PrimitiveType::Rectangle11: 
                case PrimitiveType::TransparentTexturedRectangle:
                {
                    textured_rectangles.push_back(read<tr_face4>(stream));
                    break;
                }
            }
        }

        mesh.textured_rectangles = convert_rectangles(textured_rectangles);
        mesh.textured_triangles = convert_triangles(textured_triangles);
        mesh.coloured_rectangles = coloured_rectangles;
        mesh.coloured_triangles = coloured_triangles;
    }

    void Level::load_tr1_pc_may_1996_wad(std::vector<uint8_t>& textile_buffer, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        std::filesystem::path wad_filename{ _filename };
        wad_filename.replace_extension("WAD");
        const auto wad_bytes = _files->load_file(wad_filename.string());
        if (!wad_bytes.has_value())
        {
            return;
        }

        std::basic_ispanstream<uint8_t> wad_file{ std::span(*wad_bytes) };
        wad_file.exceptions(std::ios::failbit);
        log_file(activity, wad_file, std::format("Opened file \"{}\"", wad_filename.string()));

        // Version.
        read<uint32_t>(wad_file);
        _object_textures = read_vector<uint32_t, tr_object_texture_may_1996>(wad_file)
            | std::views::transform(convert_object_texture)
            | std::ranges::to<std::vector>();

        textile_buffer.append_range(read_vector<uint32_t, uint8_t>(wad_file));

        _mesh_pointers = read_mesh_pointers(activity, wad_file, callbacks);
        _mesh_data = read_mesh_data(activity, wad_file, callbacks);

        uint32_t num_animations = read<uint32_t>(wad_file);
        skip(wad_file, num_animations * 26);

        _state_changes = read_state_changes(activity, wad_file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, wad_file, callbacks);
        read_anim_commands(activity, wad_file, callbacks);
        _meshtree = read_meshtree(activity, wad_file, callbacks);
        _frames = read_frames(activity, wad_file, callbacks);
        _models = read_models_tr1_4(activity, wad_file, callbacks);

        auto static_meshes = read_vector<uint32_t, tr_staticmesh_may_1996>(wad_file);
        log_file(activity, wad_file, std::format("Read {} static meshes", static_meshes.size()));
        for (const auto& mesh : static_meshes)
        {
            const tr_staticmesh new_mesh
            {
                .ID = mesh.ID,
                .Mesh = mesh.Mesh,
                .VisibilityBox = mesh.VisibilityBox,
                .CollisionBox = mesh.VisibilityBox,
                .Flags = 0,
            };
            _static_meshes.insert({ mesh.ID, new_mesh });
        }
    }

    void Level::load_tr1_pc_may_1996_swd(std::vector<uint8_t>& textile_buffer, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        std::filesystem::path swd_filename{ _filename };
        swd_filename.replace_extension("SWD");
        const auto wad_bytes = _files->load_file(swd_filename);
        if (!wad_bytes.has_value())
        {
            return;
        }

        std::basic_ispanstream<uint8_t> wad_file{ std::span(*wad_bytes) };
        wad_file.exceptions(std::ios::failbit);
        log_file(activity, wad_file, std::format("Opened file \"{}\"", swd_filename.string()));

        // Version.
        read<uint32_t>(wad_file);

        uint16_t base = static_cast<uint16_t>(textile_buffer.size() / sizeof(tr_textile8));
        auto sprite_textures = read_vector<uint32_t, tr_sprite_texture_may_1996>(wad_file);
        _sprite_textures =  sprite_textures |
            std::views::transform([=](auto&& t) -> tr_sprite_texture
                {
                    return { .Tile = static_cast<uint16_t>(base + t.Tile), .x = t.x, .y = t.y, .Width = t.Width, .Height = t.Height, .LeftSide = t.LeftSide, .TopSide = t.TopSide, .RightSide = t.RightSide, .BottomSide = t.BottomSide };
                }) | std::ranges::to<std::vector>();

        textile_buffer.append_range(read_vector<uint32_t, uint8_t>(wad_file));
        _sprite_sequences = read_sprite_sequences(activity, wad_file, callbacks);
    }

    void Level::load_tr1_pc_version_21(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        auto room_object_textures = read_vector<uint16_t, tr_object_texture_may_1996>(file);

        std::vector<uint8_t> textile_buffer;
        auto room_textiles = read_vector<uint32_t, uint8_t>(file);

        read_palette_tr1(file, activity, callbacks);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_pc_room_version_21);
        _floor_data = read_floor_data(activity, file, callbacks);

        load_tr1_pc_may_1996_wad(textile_buffer, activity, callbacks);
        load_tr1_pc_may_1996_swd(textile_buffer, activity, callbacks);

        adjust_room_textures();

        const uint32_t before_room_size = static_cast<uint32_t>(_object_textures.size());
        auto new_room_textures = room_object_textures | std::views::transform(convert_object_texture) | std::ranges::to<std::vector>();
        for (auto& texture : new_room_textures)
        {
            texture.TileAndFlag += static_cast<uint16_t>(textile_buffer.size() / sizeof(tr_textile8));
        }
        _object_textures.append_range(new_room_textures);
        new_room_textures = {};

        textile_buffer.append_range(room_textiles);

        _cameras = read_cameras(activity, file, callbacks);
        const auto boxes = read_boxes_tr1(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        skip(file, static_cast<uint32_t>(boxes.size()) * 4);
        _animated_textures = read_animated_textures_21(activity, file, callbacks, before_room_size);

        _entities = read_entities_tr1(activity, file, callbacks);
        read_light_map(activity, file, callbacks);

        // Assemble textiles:
        const uint32_t full_textile_size = static_cast<uint32_t>(textile_buffer.size());
        _textile8.resize(full_textile_size / sizeof(tr_textile8) + ((full_textile_size % sizeof(tr_textile8) > 0) ? 1 : 0));
        memcpy(&_textile8[0], &textile_buffer[0], textile_buffer.size());
        _num_textiles = static_cast<uint32_t>(_textile8.size());

        // Object textures don't have attributes for transparency so must be calculated.
        for (auto& ot : _object_textures)
        {
            ot.Attribute = attribute_for_object_texture(ot, _textile8);
        }

        generate_textiles_from_textile8(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::load_tr1_pc_may_1996(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        skip(file, 4); // version number
        auto room_object_textures = read_vector<uint16_t, tr_object_texture_may_1996>(file);

        std::vector<uint8_t> textile_buffer;
        auto room_textiles = read_vector<uint32_t, uint8_t>(file);

        read_palette_tr1(file, activity, callbacks);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_pc_room_may_1996);
        _floor_data = read_floor_data(activity, file, callbacks);

        load_tr1_pc_may_1996_wad(textile_buffer, activity, callbacks);
        load_tr1_pc_may_1996_swd(textile_buffer, activity, callbacks);

        adjust_room_textures();

        const uint32_t before_room_size = static_cast<uint32_t>(_object_textures.size());
        auto new_room_textures = room_object_textures | std::views::transform(convert_object_texture) | std::ranges::to<std::vector>();
        for (auto& texture : new_room_textures)
        {
            texture.TileAndFlag += static_cast<uint16_t>(textile_buffer.size() / sizeof(tr_textile8));
        }
        _object_textures.append_range(new_room_textures);
        new_room_textures = {};

        textile_buffer.append_range(room_textiles);

        if (_platform_and_version.raw_version == 9)
        {
            _entities = read_vector<uint32_t, tr_entity_may_1996>(file) |
                std::views::transform([](auto&& e) -> tr2_entity {
                return
                {
                    .TypeID = e.TypeID,
                    .Room = e.Room,
                    .x = e.x,
                    .y = e.y,
                    .z = e.z,
                    .Angle = e.Angle,
                    .Intensity1 = 0,
                    .Intensity2 = 0,
                    .Flags = e.Flags
                };}) | std::ranges::to<std::vector>();
        }
        else
        {
            skip(file, 4);
            read_boxes_tr1(activity, file, callbacks);
            read_overlaps(activity, file, callbacks);
            _animated_textures = read_animated_textures_21(activity, file, callbacks, before_room_size);

            _entities = read_vector<uint32_t, tr_entity_may_1996>(file) |
                std::views::transform([](auto&& e) -> tr2_entity {
                return
                {
                    .TypeID = e.TypeID,
                    .Room = e.Room,
                    .x = e.x,
                    .y = e.y,
                    .z = e.z,
                    .Angle = e.Angle,
                    .Intensity1 = 0,
                    .Intensity2 = 0,
                    .Flags = e.Flags
                };}) | std::ranges::to<std::vector>();
        }

        // Assemble textiles:
        const uint32_t full_textile_size = static_cast<uint32_t>(textile_buffer.size());
        _textile8.resize(full_textile_size / sizeof(tr_textile8) + ((full_textile_size % sizeof(tr_textile8) > 0) ? 1 : 0));
        memcpy(&_textile8[0], &textile_buffer[0], textile_buffer.size());
        _num_textiles = static_cast<uint32_t>(_textile8.size());

        // Object textures don't have attributes for transparency so must be calculated.
        for (auto& ot : _object_textures)
        {
            ot.Attribute = attribute_for_object_texture(ot, _textile8);
        }

        generate_textiles_from_textile8(callbacks);

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::load_tr1_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        if (is_tr1_version_21(_platform_and_version))
        {
            return load_tr1_pc_version_21(file, activity, callbacks);
        }

        if (is_tr1_may_1996(_platform_and_version))
        {
            return load_tr1_pc_may_1996(file, activity, callbacks);
        }

        skip(file, 4); // version number
        read_textiles_tr1_pc(file, activity, callbacks);
        read<uint32_t>(file);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_pc_room);
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

        // Special handling for demo levels or levels where the palette is here:
        bool on_demo_attempt = false;
        const auto branch_start = file.tellg();
        std::optional<std::exception> original_exception;

        do
        {
            try
            {
                if (on_demo_attempt)
                {
                    read_palette_tr1(file, activity, callbacks);
                }

                _cameras = read_cameras(activity, file, callbacks);
                _sound_sources = read_sound_sources(activity, file, callbacks);
                const auto boxes = read_boxes_tr1(activity, file, callbacks);
                read_overlaps(activity, file, callbacks);
                read_zones_tr1(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
                _animated_textures = read_animated_textures(activity, file, callbacks);
                _entities = read_entities_tr1(activity, file, callbacks);
                read_light_map(activity, file, callbacks);

                if (!on_demo_attempt)
                {
                    read_palette_tr1(file, activity, callbacks);
                }

                generate_textiles_from_textile8(callbacks);

                read_cinematic_frames(activity, file, callbacks);
                read_demo_data(activity, file, callbacks);
                _sound_map = read_sound_map(activity, file, callbacks);
                _sound_details = read_sound_details(activity, file, callbacks);
                _sound_data = read_sound_data(activity, file, callbacks);
                _sample_indices = read_sample_indices(activity, file, callbacks);
                break;
            }
            catch (const std::exception& e)
            {
                if (!on_demo_attempt)
                {
                    original_exception = e;
                    callbacks.on_progress("Attempting to load as TR1 demo");
                    file.clear();
                    file.seekg(branch_start, std::ios::beg);
                    on_demo_attempt = true;
                }
                else
                {
                    throw original_exception.value_or(e);
                }
            }

        } while (on_demo_attempt);

        generate_sound_samples(callbacks);
        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::read_palette_tr1(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading 8-bit palette");
        log_file(activity, file, "Reading 8-bit palette");
        _palette = read_vector<tr_colour>(file, 256);
        log_file(activity, file, "Read 8-bit palette");
    }

    void Level::read_textiles_tr1_pc(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");
        _num_textiles = read<uint32_t>(file);
        callbacks.on_progress(std::format("Reading {} 8-bit textiles", _num_textiles));
        log_file(activity, file, std::format("Reading {} 8-bit textiles", _num_textiles));
        _textile8 = read_vector<tr_textile8>(file, _num_textiles);
    }

    void Level::generate_textiles_from_textile8(const LoadCallbacks& callbacks)
    {
        for (const auto& t : _textile8)
        {
            callbacks.on_textile(t.Tile |
                std::views::transform([&](uint8_t entry_index)
                    {
                        // The first entry in the 8 bit palette is the transparent colour, so just return 
                        // fully transparent instead of replacing it later.
                        if (entry_index == 0)
                        {
                            return 0x00000000u;
                        }
                        auto entry = get_palette_entry(entry_index);
                        uint32_t value = 0xff000000 | entry.Blue << 16 | entry.Green << 8 | entry.Red;
                        return value;
                    }) | std::ranges::to<std::vector>());
        }
        _textile8 = {};
    }
}
