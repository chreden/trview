#include "Level.h"
#include "Level_common.h"
#include "Level_tr1.h"
#include "Level_psx.h"

#include <ranges>
#include <format>
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
#pragma pack(pop)

        std::vector<tr_model> read_models_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
        {
            callbacks.on_progress("Reading models");
            log_file(activity, file, "Reading models");
            auto models = convert_models(read_vector<uint32_t, tr_model_psx>(file));
            log_file(activity, file, std::format("Read {} models", models.size()));
            return models;
        }

        void read_room_lights_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading lights");
            room.lights = convert_lights(read_vector<uint16_t, tr_room_light_psx>(file));
            log_file(activity, file, std::format("Read {} lights", room.lights.size()));
        }

        void read_room_static_meshes_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            log_file(activity, file, "Reading static meshes");
            room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh_psx>(file));
            log_file(activity, file, std::format("Read {} static meshes", room.static_meshes.size()));
        }

        void load_tr1_psx_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info = read_room_info(activity, file);
            uint32_t NumDataWords = read_num_data_words(activity, file);
            skip(file, 2);

            if (NumDataWords > 0)
            {
                read_room_vertices_tr1_psx(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);
                read_room_sprites(activity, file, room);

                for (auto& rectangle : room.data.rectangles)
                {
                    std::swap(rectangle.vertices[2], rectangle.vertices[3]);
                }
            }

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_lights_tr1_psx(activity, file, room);
            read_room_static_meshes_tr1_psx(activity, file, room);
            read_room_alternate_room(activity, file, room);
            read_room_flags(activity, file, room);
        }

        void load_tr1_psx_may_1996_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
        {
            room.info.x = read<int32_t>(file);
            room.info.z = read<int32_t>(file);

            uint32_t NumDataWords = read_num_data_words(activity, file);
            skip(file, 2);
            if (NumDataWords > 0)
            {
                read_room_vertices_tr1_psx(activity, file, room);
                read_room_rectangles(activity, file, room);
                read_room_triangles(activity, file, room);
                read_room_sprites(activity, file, room);

                for (auto& rectangle : room.data.rectangles)
                {
                    std::swap(rectangle.vertices[2], rectangle.vertices[3]);
                }
            }

            auto [min, max] = std::ranges::minmax(room.data.vertices | std::views::transform([](auto&& v) { return v.vertex.y; }));
            room.info.yTop = min;
            room.info.yBottom = max;

            read_room_portals(activity, file, room);
            read_room_sectors(activity, file, room);
            read_room_ambient_intensity_1(activity, file, room);
            read_room_lights_tr1_psx(activity, file, room);
            read_room_static_meshes_tr1_psx(activity, file, room);
            read_room_flags(activity, file, room);
        }

        void read_zones_tr1_version_27(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks, uint32_t num_boxes)
        {
            callbacks.on_progress("Reading zones");
            log_file(activity, file, "Reading zones");
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 4);
            log_file(activity, file, std::format("Read {} zones", zones.size()));
        }
    }

    void read_room_vertices_tr1_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
    {
        log_file(activity, file, "Reading vertices");
        room.data.vertices = convert_vertices(convert_psx_vertex_lighting(read_vector<int16_t, tr_room_vertex>(file)));
        log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));
    }

    void Level::generate_mesh_tr1_psx_may_1996(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        mesh.centre = { .x = 0, .y = 0, .z = 0 };
        mesh.coll_radius = read<uint16_t>(stream);
        int16_t vertices_count = read<int16_t>(stream);
        int16_t normals_count = vertices_count;
        vertices_count = static_cast<int16_t>(std::abs(vertices_count));
        
        mesh.vertices = convert_vertices(read_vector<tr_vertex_psx>(stream, vertices_count));
        
        for (int i = 0; i < vertices_count; ++i)
        {
            if (normals_count > 0)
            {
                tr_vertex_psx norm = read<tr_vertex_psx>(stream);
                norm.w = 1;
                mesh.normals.push_back({ norm.x, norm.y, norm.z });
            }
            else
            {
                mesh.lights.push_back(read<int16_t>(stream)); // intensity
                mesh.normals.push_back({ 0, 0, 0 });
            }
        }
        const auto rectangles = read_vector<int16_t, tr_face4>(stream);
        const auto triangles = read_vector<int16_t, tr_face3>(stream);

        mesh.textured_rectangles = rectangles
            | std::views::filter([](const auto& rect) { return rect.texture > 255; })
            | std::views::transform([](const auto& rect)
                {
                    tr4_mesh_face4 new_face4;
                    memcpy(new_face4.vertices, rect.vertices, sizeof(rect.vertices));
                    new_face4.texture = rect.texture;
                    new_face4.effects = 0;
                    return new_face4;
                })
            | std::ranges::to<std::vector>();
        mesh.textured_triangles = triangles
            | std::views::filter([](const auto& tri) { return tri.texture > 255; })
            | std::views::transform([](const auto& tri)
                {
                    tr4_mesh_face3 new_face3;
                    memcpy(new_face3.vertices, tri.vertices, sizeof(tri.vertices));
                    new_face3.texture = tri.texture;
                    new_face3.effects = 0;
                    return new_face3;
                })
            | std::ranges::to<std::vector>();

        mesh.coloured_rectangles = rectangles
            | std::views::filter([](const auto& rect) { return rect.texture < 256; })
            | std::ranges::to<std::vector>();

        mesh.coloured_triangles = triangles
            | std::views::filter([](const auto& tri) { return tri.texture < 256; })
            | std::ranges::to<std::vector>();
    }

    void Level::generate_mesh_tr1_psx(tr_mesh& mesh, std::basic_ispanstream<uint8_t>& stream)
    {
        mesh.centre = read<tr_vertex>(stream);
        mesh.coll_radius = read<int32_t>(stream);
        int16_t vertices_count = read<int16_t>(stream);
        int16_t normals_count = vertices_count;
        vertices_count = static_cast<int16_t>(std::abs(vertices_count));

        mesh.vertices = convert_vertices(read_vector<tr_vertex_psx>(stream, vertices_count));

        for (int i = 0; i < vertices_count; ++i)
        {
            if (normals_count > 0)
            {
                tr_vertex_psx norm = read<tr_vertex_psx>(stream);
                norm.w = 1;
                mesh.normals.push_back({ norm.x, norm.y, norm.z });
            }
            else
            {
                mesh.lights.push_back(read<int16_t>(stream)); // intensity
                mesh.normals.push_back({ 0, 0, 0 });
            }
        }

        const auto rectangles = read_vector<int16_t, tr_face4>(stream);
        const auto triangles = read_vector<int16_t, tr_face3>(stream);

        mesh.textured_rectangles = rectangles
            | std::views::filter([](const auto& rect) { return rect.texture > 255; })
            | std::views::transform([](const auto& rect)
                {
                    tr4_mesh_face4 new_face4;
                    memcpy(new_face4.vertices, rect.vertices, sizeof(rect.vertices));
                    new_face4.texture = rect.texture;
                    new_face4.effects = 0;
                    return new_face4;
                })
            | std::ranges::to<std::vector>();
        mesh.textured_triangles = triangles
            | std::views::filter([](const auto& tri) { return tri.texture > 255; })
            | std::views::transform([](const auto& tri)
                {
                    tr4_mesh_face3 new_face3;
                    memcpy(new_face3.vertices, tri.vertices, sizeof(tri.vertices));
                    new_face3.texture = tri.texture;
                    new_face3.effects = 0;
                    return new_face3;
                })
            | std::ranges::to<std::vector>();

        mesh.coloured_rectangles = rectangles
            | std::views::filter([](const auto& rect) { return rect.texture < 256; })
            | std::ranges::to<std::vector>();

        mesh.coloured_triangles = triangles
            | std::views::filter([](const auto& tri) { return tri.texture < 256; })
            | std::ranges::to<std::vector>();
    }

    void Level::load_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(0, std::ios::beg);
        uint32_t sound_header_size = read<uint32_t>(file);
        if (sound_header_size && peek<uint32_t>(file) == 0x56414270) // pBAV
        {
            skip(file, 12);
            uint32_t textile_address = read<uint32_t>(file);
            skip(file, 2);
            read_sounds_tr1_psx(file, activity, callbacks, 11025);
            file.seekg(textile_address + 8, std::ios::beg);
        }
        else if (!sound_header_size)
        {
            const uint32_t sound_data_size = read<uint32_t>(file);
            skip(file, sound_data_size);
        }
        else
        {
            read_sounds_external_tr1_psx(activity, callbacks);
            file.seekg(0, std::ios::beg);
        }

        if (is_tr1_version_27(_platform_and_version))
        {
            return load_tr1_psx_version_27(file, activity, callbacks);
        }

        if (is_tr1_may_1996(_platform_and_version))
        {
            return load_tr1_psx_may_1996(file, activity, callbacks);
        }

        const uint32_t before_textiles = static_cast<uint32_t>(file.tellg());
        read_textiles_tr1_psx(file, activity, callbacks);

        // The 'without sound' variant loading also gets triggered with some of the cutscenes so
        // if we aren't at the version number, try offsetting back and reading textiles again.
        uint32_t version = read<uint32_t>(file);
        if (version != 32)
        {
            file.seekg(before_textiles - 8);
            read_textiles_tr1_psx(file, activity, callbacks);
            version = read<uint32_t>(file);;
        }

        if (file.eof())
        {
            // VICT.TR2 ends here.
            return;
        }

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_psx_room);
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
        read_object_textures_tr1_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes_tr1(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones_tr1(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities_tr1(activity, file, callbacks);
        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);

        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::read_textiles_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");

        _num_textiles = 13;
        _textile4 = read_vector<tr_textile4>(file, 13);
        _clut = read_vector<tr_clut>(file, 1024);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
    }

    void Level::read_textiles_tr1_psx_version_27(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading textiles");
        log_file(activity, file, "Reading textiles");

        _num_textiles = 15;
        _textile4 = read_vector<tr_textile4>(file, _num_textiles);
        _clut = read_vector<tr_clut>(file, 1024);
        log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
    }

    void Level::read_object_textures_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading object textures");
        log_file(activity, file, "Reading object textures");
        _object_textures_psx = read_vector<uint32_t, tr_object_texture_psx>(file);
        _object_textures = _object_textures_psx
            | std::views::transform([&](const auto texture)
                {
                    tr_object_texture_psx new_texture = texture;
                    new_texture.Tile = convert_textile4(texture.Tile, texture.Clut);
                    new_texture.Attribute = attribute_for_clut(texture.Clut);
                    new_texture.Clut = 0U; // Unneeded after conversion
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
                            { 0, texture.x3, 0, texture.y3 }
                        }
                    };
                })
            | std::ranges::to<std::vector>();
        log_file(activity, file, std::format("Read {} object textures", _object_textures.size()));
    }

    void Level::read_sounds_external_tr1_psx(trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        try
        {
            const std::string folder = trview::path_for_filename(_filename);
            const std::string level_filename = trview::filename_without_path(_filename);
            auto dot_pos = level_filename.find_last_of('.');
            const std::string level_name = dot_pos == level_filename.npos ? level_filename : level_filename.substr(0, dot_pos);

            auto sound_header = _files->load_file(std::format("{}\\..\\PSXSOUND\\{}.VBH", folder, level_name));
            auto sound_data = _files->load_file(std::format("{}\\..\\PSXSOUND\\{}.VBB", folder, level_name));

            if (sound_header && sound_data)
            {
                const uint32_t data_size = static_cast<uint32_t>(sound_data->size());
                sound_header->resize(sound_header->size() + 4);
                memcpy(&sound_header.value()[sound_header->size() - 4], &data_size, 4);
                sound_header->append_range(sound_data.value());

                std::basic_ispanstream<uint8_t> sound_file{ std::span(*sound_header) };
                sound_file.exceptions(std::ios::failbit);

                skip(sound_file, 18);
                read_sounds_tr1_psx(sound_file, activity, callbacks, 11025);
            }
        }
        catch(std::exception&)
        {
            activity.log("Failed to read external sound data");
        }
    }

    void Level::load_tr1_psx_version_27(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(0, std::ios::beg);
        read_textiles_tr1_psx_version_27(file, activity, callbacks);
        skip(file, 4); // version 27
        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_psx_room);
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
        read_object_textures_tr1_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        _sound_sources = read_sound_sources(activity, file, callbacks);
        const auto boxes = read_boxes_tr1(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        read_zones_tr1_version_27(activity, file, callbacks, static_cast<uint32_t>(boxes.size()));
        _animated_textures = read_animated_textures(activity, file, callbacks);
        _entities = read_entities_tr1(activity, file, callbacks);
        _sound_map = read_sound_map(activity, file, callbacks);
        _sound_details = read_sound_details(activity, file, callbacks);
        generate_sounds(callbacks);
        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }

    void Level::load_tr1_psx_may_1996(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const LoadCallbacks& callbacks)
    {
        file.seekg(0, std::ios::beg);
        _num_textiles = 20;
        _textile4 = read_vector<tr_textile4>(file, _num_textiles);
        _clut = read_vector<tr_clut>(file, 2048);

        // version (11)
        skip(file, 4);

        // Palette?
        skip(file, 768);

        _rooms = read_rooms<uint16_t>(activity, file, callbacks, load_tr1_psx_may_1996_room);
        _floor_data = read_floor_data(activity, file, callbacks);
        _mesh_data = read_mesh_data(activity, file, callbacks);
        _mesh_pointers = read_mesh_pointers(activity, file, callbacks);

        uint32_t num_animations = read<uint32_t>(file); // TODO: Read animations
        skip(file, num_animations * 28);

        _state_changes = read_state_changes(activity, file, callbacks);
        _anim_dispatches = read_anim_dispatches(activity, file, callbacks);
        read_anim_commands(activity, file, callbacks);
        _meshtree = read_meshtree(activity, file, callbacks);
        _frames = read_frames(activity, file, callbacks);
        _models = read_models_psx(activity, file, callbacks);

        auto static_meshes = read_vector<uint32_t, tr_staticmesh_may_1996>(file);
        log_file(activity, file, std::format("Read {} static meshes", static_meshes.size()));
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

        read_object_textures_tr1_psx(file, activity, callbacks);
        read_sprite_textures_psx(file, activity, callbacks);

        for (const auto& t : _textile16)
        {
            callbacks.on_textile(convert_textile(t));
        }

        _sprite_sequences = read_sprite_sequences(activity, file, callbacks);
        _cameras = read_cameras(activity, file, callbacks);
        read_boxes_tr1(activity, file, callbacks);
        read_overlaps(activity, file, callbacks);
        _animated_textures = read_animated_textures(activity, file, callbacks);

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
                };
            }) | std::ranges::to<std::vector>();

        callbacks.on_progress("Generating meshes");
        generate_meshes(_mesh_data);
        callbacks.on_progress("Loading complete");
    }
}
