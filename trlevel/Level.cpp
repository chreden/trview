#include "Level.h"
#include "LevelLoadException.h"
#include "LevelEncryptedException.h"
#include <format>
#include <ranges>
#include <spanstream>

namespace trlevel
{
    ILevel::~ILevel()
    {
    }

    void ILevel::LoadCallbacks::on_progress(const std::string& message) const
    {
        if (on_progress_callback)
        {
            on_progress_callback(message);
        }
    }

    void ILevel::LoadCallbacks::on_textile(const std::vector<uint32_t>& data) const
    {
        if (on_textile_callback)
        {
            on_textile_callback(data);
        }
    }

    namespace
    {
        const float PiMul2 = 6.283185307179586476925286766559f;
        const int16_t Lara = 0;
        const int16_t LaraSkinTR3 = 315;
        const int16_t LaraSkinPostTR3 = 8;
    }

    namespace
    {
        void skip(std::basic_ispanstream<uint8_t>& file, uint32_t size)
        {
            file.seekg(size, std::ios::cur);
        }

        void skip_xela(std::basic_ispanstream<uint8_t>& file)
        {
            skip(file, 4);
        }

        template <typename T>
        void read(std::basic_ispanstream<uint8_t>& file, T& value)
        {
            file.read(reinterpret_cast<uint8_t*>(&value), sizeof(value));
        }

        template <typename T>
        T read(std::basic_ispanstream<uint8_t>& file)
        {
            T value;
            read<T>(file, value);
            return value;
        }

        template < typename DataType, typename SizeType >
        std::vector<DataType> read_vector(std::basic_ispanstream<uint8_t>& file, SizeType size)
        {
            std::vector<DataType> data(size);
            for (SizeType i = 0; i < size; ++i)
            {
                read<DataType>(file, data[i]);
            }
            return data;
        }

        template < typename SizeType, typename DataType >
        std::vector<DataType> read_vector(std::basic_ispanstream<uint8_t>& file)
        {
            auto size = read<SizeType>(file);
            return read_vector<DataType, SizeType>(file, size);
        }

        template < typename DataType, typename SizeType >
        void stream_vector(std::basic_ispanstream<uint8_t>& file, SizeType size, const std::function<void (const DataType&)>& out)
        {
            for (SizeType i = 0; i < size; ++i)
            {
                out(read<DataType>(file));
            }
        }

        std::vector<uint8_t> read_compressed(std::basic_ispanstream<uint8_t>& file)
        {
            auto uncompressed_size = read<uint32_t>(file);
            auto compressed_size = read<uint32_t>(file);
            auto compressed = read_vector<uint8_t>(file, compressed_size);
            std::vector<uint8_t> uncompressed_data(uncompressed_size);

            z_stream stream;
            memset(&stream, 0, sizeof(stream));
            int result = inflateInit(&stream);
            // Exception...
            stream.avail_in = compressed_size;
            stream.next_in = &compressed[0];
            stream.avail_out = uncompressed_size;
            stream.next_out = &uncompressed_data[0];
            result = inflate(&stream, Z_NO_FLUSH);
            inflateEnd(&stream);

            return uncompressed_data;
        }

        template < typename DataType >
        std::vector<DataType> read_vector_compressed(std::basic_ispanstream<uint8_t>& file, uint32_t elements)
        {
            auto uncompressed_data = read_compressed(file);
            std::basic_ispanstream<uint8_t> data_stream{ { uncompressed_data } };
            data_stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
            return read_vector<DataType>(data_stream, elements);
        }

        void skip_vector_compressed(std::basic_ispanstream<uint8_t>& file)
        {
            skip(file, sizeof(uint32_t));
            auto compressed_size = read<uint32_t>(file);
            skip(file, compressed_size);
        }

        void log_file(trview::Activity& activity, std::basic_ispanstream<uint8_t>& stream, const std::string& text)
        {
            activity.log(std::format("[{}] {}", static_cast<uint64_t>(stream.tellg()), text));
        }

        void log_file(trview::Activity& activity, std::istream& stream, const std::string& text)
        {
            activity.log(std::format("[{}] {}", static_cast<uint64_t>(stream.tellg()), text));
        }

        bool is_tr5(trview::Activity& activity, LevelVersion version, const std::wstring& filename)
        {
            if (version != LevelVersion::Tomb4)
            {
                return false;
            }

            activity.log("Checking file extension to determine whether this is a Tomb5 level");
            std::wstring transformed;
            std::transform(filename.begin(), filename.end(), std::back_inserter(transformed), towupper);
            return transformed.find(L".TRC") != filename.npos;
        }

        void load_tr1_4_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room, PlatformAndVersion platform_and_version)
        {
            log_file(activity, file, "Reading room info");
            room.info = convert_room_info(read<tr1_4_room_info>(file));
            log_file(activity, file, "Read room info");

            log_file(activity, file, "Reading number of data words");
            uint32_t NumDataWords = read<uint32_t>(file);
            log_file(activity, file, std::format("{} data words to process", NumDataWords));

            if (platform_and_version.platform == Platform::PSX &&
                platform_and_version.version == LevelVersion::Tomb1)
            {
                skip(file, 2);
            }

            // Read actual room data.
            if (NumDataWords > 0)
            {
                log_file(activity, file, "Reading vertices");
                if (platform_and_version.version == LevelVersion::Tomb1)
                {
                    room.data.vertices = convert_vertices(read_vector<int16_t, tr_room_vertex>(file));
                }
                else if (platform_and_version.version == LevelVersion::Tomb2)
                {
                    room.data.vertices = convert_vertices(read_vector<int16_t, tr2_room_vertex>(file));
                }
                else
                {
                    room.data.vertices = convert_vertices(read_vector<int16_t, tr3_room_vertex>(file));
                }
                log_file(activity, file, std::format("Read {} vertices", room.data.vertices.size()));

                log_file(activity, file, "Reading rectangles");
                room.data.rectangles = convert_rectangles(read_vector<int16_t, tr_face4>(file));
                log_file(activity, file, std::format("Read {} rectangles", room.data.rectangles.size()));
                log_file(activity, file, "Reading triangles");
                room.data.triangles = convert_triangles(read_vector<int16_t, tr_face3>(file));
                log_file(activity, file, std::format("Read {} triangles", room.data.triangles.size()));
                log_file(activity, file, "Reading sprites");
                room.data.sprites = read_vector<int16_t, tr_room_sprite>(file);
                log_file(activity, file, std::format("Read {} sprites", room.data.sprites.size()));

                if (platform_and_version.platform == Platform::PSX)
                {
                    for (auto& rectangle : room.data.rectangles)
                    {
                        std::swap(rectangle.vertices[2], rectangle.vertices[3]);
                    }
                }
            }

            log_file(activity, file, "Reading portals");
            room.portals = read_vector<uint16_t, tr_room_portal>(file);
            log_file(activity, file, std::format("Read {} portals", room.portals.size()));

            log_file(activity, file, "Reading number of z sectors");
            room.num_z_sectors = read<uint16_t>(file);
            log_file(activity, file, std::format("There are {} z sectors", room.num_z_sectors));
            log_file(activity, file, "Reading number of x sectors");
            room.num_x_sectors = read<uint16_t>(file);
            log_file(activity, file, std::format("There are {} x sectors", room.num_x_sectors));
            log_file(activity, file, std::format("Reading {} sectors", room.num_z_sectors * room.num_x_sectors));
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
            log_file(activity, file, std::format("Read {} sectors", room.sector_list.size()));

            if (platform_and_version.version == LevelVersion::Tomb4)
            {
                log_file(activity, file, "Reading room colour");
                room.colour = read<uint32_t>(file);
                log_file(activity, file, std::format("Read room colour {:X}", room.colour));
            }
            else
            {
                log_file(activity, file, "Reading ambient intensity 1");
                room.ambient_intensity_1 = read<int16_t>(file);
                log_file(activity, file, std::format("Read ambient intensity 1: {}", room.ambient_intensity_1));

                if (platform_and_version.version == LevelVersion::Tomb2)
                {
                    log_file(activity, file, "Reading ambient intensity 2");
                    room.ambient_intensity_2 = read<int16_t>(file);
                    log_file(activity, file, std::format("Read ambient intensity 2: {}", room.ambient_intensity_2));
                }
            }

            if (platform_and_version.version == LevelVersion::Tomb2 ||
                platform_and_version.version == LevelVersion::Tomb3)
            {
                log_file(activity, file, "Reading light mode");
                room.light_mode = read<int16_t>(file);
                log_file(activity, file, std::format("Read light mode: {}", room.light_mode));
            }

            log_file(activity, file, "Reading lights");
            switch (platform_and_version.version)
            {
            case LevelVersion::Tomb1:
            {
                if (platform_and_version.platform == Platform::PSX)
                {
                    room.lights = convert_lights(read_vector<uint16_t, tr_room_light_psx>(file));
                }
                else
                {
                    room.lights = convert_lights(read_vector<uint16_t, tr_room_light>(file));
                }
                break;
            }
            case LevelVersion::Tomb2:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr2_room_light>(file));
                break;
            }
            case LevelVersion::Tomb3:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr3_room_light>(file));
                break;
            }
            case LevelVersion::Tomb4:
            {
                room.lights = convert_lights(read_vector<uint16_t, tr4_room_light>(file));
                break;
            }
            }
            log_file(activity, file, std::format("Read {} lights", room.lights.size()));

            log_file(activity, file, "Reading static meshes");
            if (platform_and_version.version == LevelVersion::Tomb1)
            {
                if (platform_and_version.platform == Platform::PSX)
                {
                    room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh_psx>(file));
                }
                else
                {
                    room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh>(file));
                }
            }
            else
            {
                room.static_meshes = read_vector<uint16_t, tr3_room_staticmesh>(file);
            }
            log_file(activity, file, std::format("Read {} static meshes", room.static_meshes.size()));

            log_file(activity, file, "Reading alternate room");
            room.alternate_room = read<int16_t>(file);
            log_file(activity, file, std::format("Read alternate room: {}", room.alternate_room));
            log_file(activity, file, "Reading flags");
            room.flags = read<int16_t>(file);
            log_file(activity, file, std::format("Read flags: {:X}", room.flags));

            if (platform_and_version.version >= LevelVersion::Tomb3)
            {
                log_file(activity, file, "Reading water scheme");
                room.water_scheme = read<uint8_t>(file);
                log_file(activity, file, std::format("Read water scheme: {}", room.water_scheme));
                log_file(activity, file, "Reading reverb info");
                room.reverb_info = read<uint8_t>(file);
                log_file(activity, file, std::format("Read reverb info: {}", room.reverb_info));
                log_file(activity, file, "Reading alternate group");
                room.alternate_group = read<uint8_t>(file);
                log_file(activity, file, std::format("Read alternate group: {}", room.alternate_group));
            }
        }

        void load_tr5_room(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, tr3_room& room)
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

            log_file(activity, file, std::format("Reading {} lights", header.num_lights));
            room.lights = convert_lights(read_vector<tr5_room_light>(file, header.num_lights));

            log_file(activity, file, std::format("Reading {} fog bulbs", header.num_fog_bulbs));
            auto fog_bulbs = read_vector<tr5_fog_bulb>(file, header.num_fog_bulbs);

            log_file(activity, file, "Converting lights to fog buibs");
            uint32_t fog_bulb = 0;
            for (auto& light : room.lights)
            {
                if (*reinterpret_cast<uint32_t*>(&light.tr5.position.x) == 0xCDCDCDCD)
                {
                    const auto fog = fog_bulbs[fog_bulb++];
                    light.tr5.light_type = LightType::FogBulb;
                    light.tr5_fog = fog;
                }
            }

            file.seekg(data_start + header.start_sd_offset, std::ios::beg);

            log_file(activity, file, std::format("Reading {} sectors ({} x {})", room.num_x_sectors * room.num_z_sectors, room.num_x_sectors, room.num_z_sectors));
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
            log_file(activity, file, "Reading room portals");
            room.portals = read_vector<uint16_t, tr_room_portal>(file);
            log_file(activity, file, std::format("Read {} room portals", room.portals.size()));

            // Separator
            skip(file, 2);

            file.seekg(data_start + header.end_portal_offset, std::ios::beg);
            log_file(activity, file, std::format("Reading {} static meshes", header.num_static_meshes));
            room.static_meshes = read_vector<tr3_room_staticmesh>(file, header.num_static_meshes);

            file.seekg(data_start + header.layer_offset, std::ios::beg);
            log_file(activity, file, std::format("Reading {} layers", header.num_layers));
            auto layers = read_vector<tr5_room_layer>(file, header.num_layers);

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
    }

    Level::Level(const std::string& filename, const std::shared_ptr<trview::IFiles>& files, const std::shared_ptr<IDecrypter>& decrypter, const std::shared_ptr<trview::ILog>& log)
        : _log(log), _decrypter(decrypter), _filename(filename), _files(files)
    {
    }

    Level::~Level()
    {
    }

    void Level::generate_meshes(const std::vector<uint16_t>& mesh_data)
    {
        if (_mesh_data.empty())
        {
            return;
        }

        // As well as reading the actual mesh data, generate a map of mesh_pointer to 
        // mesh. It seems that a lot of the pointers point to the same mesh.
        std::span span{ reinterpret_cast<const uint8_t*>(&mesh_data[0]), mesh_data.size() * sizeof(uint16_t) };
        std::basic_ispanstream<uint8_t> stream{ span };
        stream.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);
        for (auto pointer : _mesh_pointers)
        {
            // Does the map already contain this mesh? If so, don't bother reading it again.
            auto found = _meshes.find(pointer);
            if (found != _meshes.end())
            {
                continue;
            }

            stream.seekg(pointer, std::ios::beg);

            tr_mesh mesh;
            mesh.centre = read<tr_vertex>(stream);
            mesh.coll_radius = read<int32_t>(stream);

            if (_platform_and_version.platform == Platform::PSX)
            {
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
            else
            {
                mesh.vertices = read_vector<int16_t, tr_vertex>(stream);

                int16_t normals = read<int16_t>(stream);
                if (normals > 0)
                {
                    mesh.normals = read_vector<tr_vertex>(stream, normals);
                }
                else
                {
                    mesh.lights = read_vector<int16_t>(stream, abs(normals));
                }

                if (get_version() < LevelVersion::Tomb4)
                {
                    mesh.textured_rectangles = convert_rectangles(read_vector<int16_t, tr_face4>(stream));
                    mesh.textured_triangles = convert_triangles(read_vector<int16_t, tr_face3>(stream));
                    mesh.coloured_rectangles = read_vector<int16_t, tr_face4>(stream);
                    mesh.coloured_triangles = read_vector<int16_t, tr_face3>(stream);
                }
                else
                {
                    mesh.textured_rectangles = read_vector<int16_t, tr4_mesh_face4>(stream);
                    mesh.textured_triangles = read_vector<int16_t, tr4_mesh_face3>(stream);
                }
            }

            _meshes.insert({ pointer, mesh });
        }
    }

    tr_colour Level::get_palette_entry8(uint32_t index) const
    {
        return _palette[index];
    }

    tr_colour4 Level::get_palette_entry_16(uint32_t index) const
    {
        return _palette16[index];
    }

    tr_colour4 Level::get_palette_entry(uint32_t index) const
    {
        if (_platform_and_version.platform == Platform::PSX &&
            _platform_and_version.version == LevelVersion::Tomb1)
        {
            return colour_from_object_texture(index);
        }

        if (index < _palette16.size())
        {
            return get_palette_entry_16(index);
        }
        return convert_to_colour4(get_palette_entry8(index));
    }

    tr_colour4 Level::get_palette_entry(uint32_t index8, uint32_t index16) const
    {
        if (index16 < _palette16.size())
        {
            return get_palette_entry_16(index16);
        }
        return convert_to_colour4(get_palette_entry8(index8));
    }

    uint32_t Level::num_rooms() const
    {
        return static_cast<uint32_t>(_rooms.size());
    }

    tr3_room Level::get_room(uint32_t index) const
    {
        return _rooms[index];
    }

    uint32_t Level::num_object_textures() const
    {
        return static_cast<uint32_t>(_object_textures.size());
    }

    tr_object_texture Level::get_object_texture(uint32_t index) const
    {
        return _object_textures[index];
    }

    uint32_t Level::num_floor_data() const
    {
        return static_cast<uint32_t>(_floor_data.size());
    }

    uint16_t Level::get_floor_data(uint32_t index) const
    {
        return _floor_data[index];
    }

    std::vector<std::uint16_t> 
    Level::get_floor_data_all() const
    {
        return _floor_data;
    }

    uint32_t Level::num_ai_objects() const
    {
        return static_cast<uint32_t>(_ai_objects.size());
    }

    tr4_ai_object Level::get_ai_object(uint32_t index) const
    {
        return _ai_objects[index];
    }

    uint32_t Level::num_entities() const
    {
        return static_cast<uint32_t>(_entities.size());
    }

    tr2_entity Level::get_entity(uint32_t index) const 
    {
        return _entities[index];
    }

    uint32_t Level::num_models() const
    {
        return static_cast<uint32_t>(_models.size());
    }

    tr_model Level::get_model(uint32_t index) const
    {
        return _models[index];
    }

    bool Level::get_model_by_id(uint32_t id, tr_model& output) const 
    {
        for (const auto& model : _models)
        {
            if (model.ID == id)
            {
                output = model;
                return true;
            }
        }
        return false;
    }

    uint32_t Level::num_static_meshes() const
    {
        return static_cast<uint32_t>(_static_meshes.size());
    }

    std::optional<tr_staticmesh> Level::get_static_mesh(uint32_t mesh_id) const
    {
        auto mesh = _static_meshes.find(mesh_id);
        if (mesh == _static_meshes.end())
        {
            return std::nullopt;
        }
        return mesh->second;
    }

    uint32_t Level::num_mesh_pointers() const
    {
        return static_cast<uint32_t>(_mesh_pointers.size());
    }

    tr_mesh Level::get_mesh_by_pointer(uint32_t mesh_pointer) const
    {
        auto index = _mesh_pointers[mesh_pointer];
        return _meshes.find(index)->second;
    }

    std::vector<tr_meshtree_node> Level::get_meshtree(uint32_t starting_index, uint32_t node_count) const
    {
        uint32_t index = starting_index;
        std::vector<tr_meshtree_node> nodes;
        for (uint32_t i = 0; i < node_count; ++i)
        {
            tr_meshtree_node node;
            node.Flags = _meshtree[index++];
            node.Offset_X = static_cast<int32_t>(_meshtree[index++]);
            node.Offset_Y = static_cast<int32_t>(_meshtree[index++]);
            node.Offset_Z = static_cast<int32_t>(_meshtree[index++]);
            nodes.push_back(node);
        }
        return nodes;
    }

    tr2_frame Level::get_frame(uint32_t frame_offset, uint32_t mesh_count) const
    {
        uint32_t offset = frame_offset;
        tr2_frame frame;
        frame.bb1x = _frames[offset++];
        frame.bb1y = _frames[offset++];
        frame.bb1z = _frames[offset++];
        frame.bb2x = _frames[offset++];
        frame.bb2y = _frames[offset++];
        frame.bb2z = _frames[offset++];
        frame.offsetx = _frames[offset++];
        frame.offsety = _frames[offset++];
        frame.offsetz = _frames[offset++];

        // Tomb Raider I has the mesh count in the frame structure - all other tombs
        // already know based on the number of meshes.
        if (get_version() == LevelVersion::Tomb1)
        {
            mesh_count = _frames[offset++];
        }

        for (uint32_t i = 0; i < mesh_count; ++i)
        {
            tr2_frame_rotation rotation;

            uint16_t next = 0;
            uint16_t data = 0;
            uint16_t mode = 0;

            // Tomb Raider I has reversed words and always uses the two word format.
            if (get_version() == LevelVersion::Tomb1)
            {
                next = _frames[offset++];
                data = _frames[offset++];
            }
            else
            {
                data = _frames[offset++];
                mode = data & 0xC000;
                if (!mode)
                {
                    next = _frames[offset++];
                }
            }

            if (mode)
            {
                float angle = 0;
                if (get_version() >= LevelVersion::Tomb4)
                {
                    angle = (data & 0x0fff) * PiMul2 / 4096.0f;
                }
                else
                {
                    angle = (data & 0x03ff) * PiMul2 / 1024.0f;
                }

                if (mode == 0x4000)
                {
                    rotation.x = angle;
                }
                else if (mode == 0x8000)
                {
                    rotation.y = angle;
                }
                else if (mode == 0xC000)
                {
                    rotation.z = angle;
                }
            }
            else
            {
                rotation.x = ((data & 0x3ff0) >> 4) * PiMul2 / 1024.0f;
                rotation.y = ((((data & 0x000f) << 6)) | ((next & 0xfc00) >> 10)) * PiMul2 / 1024.0f;
                rotation.z = (next & 0x03ff) * PiMul2 / 1024.0f;
            }
            frame.values.push_back(rotation);
        }
        return frame;
    }

    LevelVersion Level::get_version() const 
    {
        return _platform_and_version.version;
    }

    bool Level::get_sprite_sequence_by_id(int32_t sprite_sequence_id, tr_sprite_sequence& output) const
    {
        auto found_sequence = std::find_if(_sprite_sequences.begin(), _sprite_sequences.end(), [=](const auto& sequence)
        {
            return sequence.SpriteID == sprite_sequence_id; 
        });

        if (found_sequence == _sprite_sequences.end())
        {
            return false;
        }

        output = *found_sequence;
        return true;
    }

    std::optional<tr_sprite_texture> Level::get_sprite_texture(uint32_t index) const
    {
        if (index < _sprite_textures.size())
        {
            return _sprite_textures[index];
        }
        return std::nullopt;
    }

    void Level::load_tr4(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const LoadCallbacks& callbacks)
    {
        log_file(activity, file, "Reading textile counts");
        uint16_t num_room_textiles = read<uint16_t>(file);
        uint16_t num_obj_textiles = read<uint16_t>(file);
        uint16_t num_bump_textiles = read<uint16_t>(file);
        log_file(activity, file, std::format("Textile counts - Room:{}, Object:{}, Bump:{}", num_room_textiles, num_obj_textiles, num_bump_textiles));
        _num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;

        callbacks.on_progress(std::format("Reading {} 32-bit textiles", _num_textiles));
        log_file(activity, file, std::format("Reading {} 32-bit textiles", _num_textiles));
        auto textile32 = read_vector_compressed<tr_textile32>(file, _num_textiles);

        constexpr auto is_blank = [](const auto& t)
            {
                for (const uint32_t& v : t.Tile)
                {
                    if (v)
                    {
                        return false;
                    }
                }
                return true;
            };

        if (std::all_of(textile32.begin(), textile32.end(), is_blank))
        {
            activity.log(trview::Message::Status::Warning, "32-bit textiles were all blank, discarding");
            textile32 = {};
            callbacks.on_progress(std::format("Reading {} 16-bit textiles", _num_textiles));
            log_file(activity, file, std::format("Reading {} 16-bit textiles", _num_textiles));
            auto textile16 = read_vector_compressed<tr_textile16>(file, _num_textiles);

            for (const auto& textile : textile16)
            {
                callbacks.on_textile(convert_textile(textile));
            }
        }
        else
        {
            for (const auto& textile : textile32)
            {
                callbacks.on_textile(convert_textile(textile));
            }
            textile32 = {};

            callbacks.on_progress(std::format("Skipping {} 16-bit textiles", _num_textiles));
            log_file(activity, file, std::format("Skipping {} 16-bit textiles", _num_textiles));
            skip_vector_compressed(file);
        }
        
        log_file(activity, file, "Reading misc textiles");
        auto textile32_misc = read_vector_compressed<tr_textile32>(file, 2);

        if (get_version() == LevelVersion::Tomb5)
        {
            log_file(activity, file, "Reading Lara type");
            _lara_type = read<uint16_t>(file);
            log_file(activity, file, std::format("Lara type: {}", _lara_type));
            log_file(activity, file, "Reading weather type");
            _weather_type = read<uint16_t>(file);
            log_file(activity, file, std::format("Weather type: {}", _weather_type));
            log_file(activity, file, "Skipping 28 unknown/padding bytes");
            file.seekg(28, std::ios::cur);
        }

        if (get_version() == LevelVersion::Tomb4)
        {
            log_file(activity, file, "Reading and decompressing level data");
            callbacks.on_progress("Decompressing level data");
            std::vector<uint8_t> level_data = read_compressed(file);
            std::basic_ispanstream<uint8_t> data_stream{ { level_data } };
            callbacks.on_progress("Processing level data");
            load_level_data(activity, data_stream, callbacks);
        }
        else
        {
            log_file(activity, file, "Skipping uncompresed and compressed sizes - unused in Tomb5");
            skip(file, 8);
            callbacks.on_progress("Processing level data");
            load_level_data(activity, file, callbacks);
        }

        callbacks.on_progress("Skipping sound samples");
        log_file(activity, file, "Skipping sound samples");

        callbacks.on_progress("Generating meshes");
        log_file(activity, file, "Generating meshes");
        generate_meshes(_mesh_data);
    }

    void Level::load_level_data(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const LoadCallbacks& callbacks)
    {
        // Read unused value.
        read<uint32_t>(file);

        if (file.eof())
        {
            // VICT.TR2 ends here.
            return;
        }

        log_file(activity, file, "Reading number of rooms");
        uint32_t num_rooms = 0;
        if (get_version() == LevelVersion::Tomb5)
        {
            num_rooms = read<uint32_t>(file);
        }
        else
        {
            num_rooms = read<uint16_t>(file);
        }

        callbacks.on_progress(std::format("Reading {} rooms", num_rooms));
        log_file(activity, file, std::format("Reading {} rooms", num_rooms));
        for (auto i = 0u; i < num_rooms; ++i)
        {
            trview::Activity room_activity(activity, std::format("Room {}", i));
            callbacks.on_progress(std::format("Reading room {}", i));
            log_file(room_activity, file, std::format("Reading room {}", i));
            tr3_room room;
            if (get_version() == LevelVersion::Tomb5)
            {
                load_tr5_room(room_activity, file, room);
            }
            else
            {
                load_tr1_4_room(room_activity, file, room, _platform_and_version);
            }
            log_file(room_activity, file, std::format("Read room {}", i));
            _rooms.push_back(room);
        }

        callbacks.on_progress("Reading floor data");
        log_file(activity, file, "Reading floor data");
        _floor_data = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} floor data", _floor_data.size()));

        callbacks.on_progress("Reading mesh data");
        log_file(activity, file, "Reading mesh data");
        _mesh_data = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} mesh data", _mesh_data.size()));

        callbacks.on_progress("Reading mesh pointers");
        log_file(activity, file, "Reading mesh pointers");
        _mesh_pointers = read_vector<uint32_t, uint32_t>(file);
        log_file(activity, file, std::format("Read {} mesh pointers", _mesh_pointers.size()));

        callbacks.on_progress("Reading animations");
        log_file(activity, file, "Reading animations");
        if (get_version() >= LevelVersion::Tomb4)
        {
            auto animations = read_vector<uint32_t, tr4_animation>(file);
        }
        else
        {
            std::vector<tr_animation> animations = read_vector<uint32_t, tr_animation>(file);
        }

        callbacks.on_progress("Reading state changes");
        log_file(activity, file, "Reading state changes");
        std::vector<tr_state_change> state_changes = read_vector<uint32_t, tr_state_change>(file);
        log_file(activity, file, std::format("Read {} state changes", state_changes.size()));

        callbacks.on_progress("Reading anim dispatches");
        log_file(activity, file, "Reading anim dispatches");
        std::vector<tr_anim_dispatch> anim_dispatches = read_vector<uint32_t, tr_anim_dispatch>(file);
        log_file(activity, file, std::format("Read {} anim dispatches", anim_dispatches.size()));

        callbacks.on_progress("Reading anim commands");
        log_file(activity, file, "Reading anim commands");
        std::vector<tr_anim_command> anim_commands = read_vector<uint32_t, tr_anim_command>(file);
        log_file(activity, file, std::format("Read {} anim commands", anim_commands.size()));

        callbacks.on_progress("Reading mesh trees");
        log_file(activity, file, "Reading mesh trees");
        _meshtree = read_vector<uint32_t, uint32_t>(file);
        log_file(activity, file, std::format("Read {} mesh trees", _meshtree.size()));

        callbacks.on_progress("Reading frames");
        log_file(activity, file, "Reading frames");
        _frames = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} frames", _frames.size()));

        callbacks.on_progress("Reading models");
        log_file(activity, file, "Reading models");
        if (get_version() < LevelVersion::Tomb5)
        {
            _models = 
                _platform_and_version.platform == Platform::PSX ?
                    convert_models(read_vector<uint32_t, tr_model_psx>(file)) :
                    read_vector<uint32_t, tr_model>(file);
        }
        else
        {
            _models = convert_models(read_vector<uint32_t, tr5_model>(file));
        }
        log_file(activity, file, std::format("Read {} models", _models.size()));

        callbacks.on_progress("Reading static meshes");
        log_file(activity, file, "Reading static meshes");
        auto static_meshes = read_vector<uint32_t, tr_staticmesh>(file);
        log_file(activity, file, std::format("Read {} static meshes", static_meshes.size()));
        for (const auto& mesh : static_meshes)
        {
            _static_meshes.insert({ mesh.ID, mesh });
        }

        if (get_version() < LevelVersion::Tomb3)
        {
            callbacks.on_progress("Reading object textures");
            log_file(activity, file, "Reading object textures");
            if (_platform_and_version.platform == Platform::PSX)
            {
                _object_textures_psx = read_vector<uint32_t, tr_object_texture_psx>(file);
                _object_textures = _object_textures_psx
                    | std::views::transform([&](const auto texture)
                        {
                            tr_object_texture_psx new_texture = texture;
                            new_texture.Tile = convert_textile4(texture.Tile, texture.Clut);
                            new_texture.Clut = 0U; // Unneeded after conversion
                            if (new_texture.x3 || new_texture.y3)
                            {
                                std::swap(new_texture.x2, new_texture.x3);
                                std::swap(new_texture.y2, new_texture.y3);
                            }
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
            }
            else
            {
                _object_textures = read_vector<uint32_t, tr_object_texture>(file);
            }

            log_file(activity, file, std::format("Read {} object textures", _object_textures.size()));
        }

        if (get_version() >= LevelVersion::Tomb4)
        {
            log_file(activity, file, "Skipping SPR marker");
            // Skip past the 'SPR' marker.
            file.seekg(3, std::ios::cur);
            if (get_version() == LevelVersion::Tomb5)
            {
                log_file(activity, file, "Skipping SPR null terminator");
                skip(file, 1);
            }
        }

        callbacks.on_progress("Reading sprite textures");
        log_file(activity, file, "Reading sprite textures");
        if (_platform_and_version.platform == Platform::PSX)
        {
            auto textures = read_vector<uint32_t, tr_sprite_texture_psx>(file);
            _sprite_textures = textures
                | std::views::transform([&](const auto texture) -> tr_sprite_texture
                    {
                        const uint16_t tile = convert_textile4(texture.Tile, texture.Clut);
                        const uint16_t width = (texture.u1 - texture.u0) * 256 + 255;
                        const uint16_t height = (texture.v1 - texture.v0) * 256 + 255;
                        return { tile, texture.u0, texture.v0, width, height, texture.LeftSide, texture.TopSide, texture.RightSide, texture.BottomSide };
                    })
                | std::ranges::to<std::vector>();
        }
        else
        {
            _sprite_textures = read_vector<uint32_t, tr_sprite_texture>(file);
        }
        log_file(activity, file, std::format("Read {} sprite textures", _sprite_textures.size()));

        if (_platform_and_version.platform == Platform::PSX)
        {
            for (const auto& t : _textile16)
            {
                callbacks.on_textile(convert_textile(t));
            };
            _textile16 = {};
        }


        callbacks.on_progress("Reading sprite sequences");
        log_file(activity, file, "Reading sprite sequences");
        _sprite_sequences = read_vector<uint32_t, tr_sprite_sequence>(file);
        log_file(activity, file, std::format("Read {} sprite sequences", _sprite_sequences.size()));

        // If this is Unfinished Business, the palette is here.
        // Need to do something about that, instead of just crashing.

        callbacks.on_progress("Reading cameras");
        log_file(activity, file, "Reading cameras");
        _cameras = read_vector<uint32_t, tr_camera>(file);
        log_file(activity, file, std::format("Read {} cameras", _cameras.size()));

        if (get_version() >= LevelVersion::Tomb4)
        {
            callbacks.on_progress("Reading flyby cameras");
            log_file(activity, file, "Reading flyby cameras");
            std::vector<tr4_flyby_camera> flyby_cameras = read_vector<uint32_t, tr4_flyby_camera>(file);
            log_file(activity, file, std::format("Read {} flyby cameras", flyby_cameras.size()));
        }

        callbacks.on_progress("Reading sound sources");
        log_file(activity, file, "Reading sound sources");
        std::vector<tr_sound_source> sound_sources = read_vector<uint32_t, tr_sound_source>(file);
        log_file(activity, file, std::format("Read {} sound sources", sound_sources.size()));

        uint32_t num_boxes = 0;
        callbacks.on_progress("Reading boxes");
        log_file(activity, file, "Reading boxes");
        if (get_version() == LevelVersion::Tomb1)
        {
            std::vector<tr_box> boxes = read_vector<uint32_t, tr_box>(file);
            num_boxes = static_cast<uint32_t>(boxes.size());
        }
        else
        {
            std::vector<tr2_box> boxes = read_vector<uint32_t, tr2_box>(file);
            num_boxes = static_cast<uint32_t>(boxes.size());
        }
        log_file(activity, file, std::format("Read {} boxes", num_boxes));

        callbacks.on_progress("Reading overlaps");
        log_file(activity, file, "Reading overlaps");
        std::vector<uint16_t> overlaps = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} overlaps", overlaps.size()));

        callbacks.on_progress("Reading zones");
        log_file(activity, file, "Reading zones");
        if (get_version() == LevelVersion::Tomb1)
        {
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 6);
            log_file(activity, file, std::format("Read {} zones", zones.size()));
        }
        else
        {
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 10);
            log_file(activity, file, std::format("Read {} zones", zones.size()));
        }

        callbacks.on_progress("Reading animated textures");
        log_file(activity, file, "Reading animated textures");
        std::vector<uint16_t> animated_textures = read_vector<uint32_t, uint16_t>(file);
        log_file(activity, file, std::format("Read {} animated textures", animated_textures.size()));

        if (get_version() >= LevelVersion::Tomb4)
        {
            // Animated textures uv count - not yet used:
            log_file(activity, file, "Reading animated textures UV count");
            uint8_t animated_textures_uv_count = read<uint8_t>(file);
            log_file(activity, file, std::format("Animated texture UV count: {}", animated_textures_uv_count));

            log_file(activity, file, "Skipping TEX marker");
            file.seekg(3, std::ios::cur);
            if (get_version() == LevelVersion::Tomb5)
            {
                log_file(activity, file, "Skipping TEX null terminator");
                skip(file, 1);
            }
        }

        if (get_version() == LevelVersion::Tomb3)
        {
            callbacks.on_progress("Reading object textures");
            log_file(activity, file, "Reading object textures");
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
            log_file(activity, file, std::format("Read {} object textures", _object_textures.size()));
        }
        if (get_version() == LevelVersion::Tomb4)
        {
            callbacks.on_progress("Reading object textures");
            log_file(activity, file, "Reading object textures");
            _object_textures = convert_object_textures(read_vector<uint32_t, tr4_object_texture>(file));
            log_file(activity, file, std::format("Read {} object textures", _object_textures.size()));
        }
        else if (get_version() == LevelVersion::Tomb5)
        {
            callbacks.on_progress("Reading object textures");
            log_file(activity, file, "Reading object textures");
            _object_textures = convert_object_textures(read_vector<uint32_t, tr5_object_texture>(file));
            log_file(activity, file, std::format("Read {} object textures", _object_textures.size()));
        }

        callbacks.on_progress("Reading entities");
        log_file(activity, file, "Reading entities");
        if (get_version() == LevelVersion::Tomb1)
        {
            _entities = convert_entities(read_vector<uint32_t, tr_entity>(file));
        }
        else
        {
            // TR4 entity is in here, OCB is not set but goes into intensity2 (convert later).
            _entities = read_vector<uint32_t, tr2_entity>(file);
        }
        log_file(activity, file, std::format("Read {} entities", _entities.size()));

        if (get_version() < LevelVersion::Tomb4)
        {
            callbacks.on_progress("Reading light map");
            log_file(activity, file, "Reading light map");
            std::vector<uint8_t> light_map = read_vector<uint8_t>(file, 32 * 256);
            log_file(activity, file, "Read light map");
        }

        if (_platform_and_version.platform == Platform::PSX &&
            get_version() == LevelVersion::Tomb1)
        {
            return;
        }

        if (get_version() == LevelVersion::Tomb1)
        {
            callbacks.on_progress("Reading 8-bit palette");
            log_file(activity, file, "Reading 8-bit palette");
            _palette = read_vector<tr_colour>(file, 256);
            log_file(activity, file, "Read 8-bit palette");

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

        if (get_version() >= LevelVersion::Tomb4)
        {
            callbacks.on_progress("Reading AI objects");
            log_file(activity, file, "Reading AI objects");
            _ai_objects = read_vector<uint32_t, tr4_ai_object>(file);
            log_file(activity, file, std::format("Read {} AI objects", _ai_objects.size()));
        }

        if (get_version() < LevelVersion::Tomb4)
        {
            callbacks.on_progress("Reading cinematic frames");
            log_file(activity, file, "Reading cinematic frames");
            std::vector<tr_cinematic_frame> cinematic_frames = read_vector<uint16_t, tr_cinematic_frame>(file);
            log_file(activity, file, std::format("Read {} cinematic frames", cinematic_frames.size()));
        }

        callbacks.on_progress("Reading demo data");
        log_file(activity, file, "Reading demo data");
        std::vector<uint8_t> demo_data = read_vector<uint16_t, uint8_t>(file);
        log_file(activity, file, std::format("Read {} demo data", demo_data.size()));

        callbacks.on_progress("Reading sound map");
        log_file(activity, file, "Reading sound map");
        if (get_version() == LevelVersion::Tomb1)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 256);
        }
        else if (get_version() < LevelVersion::Tomb4)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
        }
        else if (get_version() == LevelVersion::Tomb4)
        {
            if (demo_data.size() == 2048)
            {
                std::vector<int16_t> sound_map = read_vector<int16_t>(file, 1024);
            }
            else
            {
                std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
            }
        }
        else
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 450);
        }
        log_file(activity, file, "Read sound map");

        callbacks.on_progress("Reading sound details");
        log_file(activity, file, "Reading sound details");
        std::vector<tr3_sound_details> sound_details = read_vector<uint32_t, tr3_sound_details>(file);
        log_file(activity, file, std::format("Read {} sound details", sound_details.size()));

        if (get_version() == LevelVersion::Tomb1)
        {
            callbacks.on_progress("Reading sound data");
            log_file(activity, file, "Reading sound data");
            std::vector<uint8_t> sound_data = read_vector<int32_t, uint8_t>(file);
            log_file(activity, file, std::format("Read {} sound data", sound_data.size()));
        }

        if (get_version() < LevelVersion::Tomb4)
        {
            callbacks.on_progress("Reading sample indices");
            log_file(activity, file, "Reading sample indices");
            std::vector<uint32_t> sample_indices = read_vector<uint32_t, uint32_t>(file);
            log_file(activity, file, std::format("Read {} sample indices", sample_indices.size()));
        }
        else
        {
            skip(file, 6);
        }
    }

    bool Level::find_first_entity_by_type(int16_t type, tr2_entity& entity) const
    {
        auto found = std::find_if(_entities.begin(), _entities.end(), [type](const auto& e) { return e.TypeID == type; });
        if (found == _entities.end())
        {
            return false;
        }
        entity = *found;
        return true;
    }

    int16_t Level::get_mesh_from_type_id(int16_t type) const
    {
        if (type != 0 || get_version() < LevelVersion::Tomb3)
        {
            return type;
        }

        if (get_version() > trlevel::LevelVersion::Tomb3)
        {
            return LaraSkinPostTR3;
        }
        return LaraSkinTR3;
    }

    std::string Level::name() const
    {
        return _name;
    }

    uint32_t Level::num_cameras() const
    {
        return static_cast<uint32_t>(_cameras.size());
    }

    tr_camera Level::get_camera(uint32_t index) const
    {
        return _cameras[index];
    }

    Platform Level::platform() const
    {
        return _platform_and_version.platform;
    }

    tr_colour4 Level::colour_from_object_texture(uint32_t texture) const
    {
        if (texture >= _object_textures_psx.size())
        {
            return tr_colour4{ .Red = 0, .Green = 0, .Blue = 0 };
        }

        const auto& object_texture = _object_textures_psx[texture];
        if (object_texture.Tile >= _textile4.size() ||
            object_texture.Clut >= _clut.size())
        {
            return tr_colour4{ .Red = 0, .Green = 0, .Blue = 0 };
        }

        const auto& tile = _textile4[object_texture.Tile];
        const auto& clut = _clut[object_texture.Clut];

        const auto pixel = object_texture.x0 + object_texture.y0 * 256;
        const auto index = tile.Tile[pixel / 2];
        const auto colour = clut.Colour[object_texture.x0 % 2 ? index.b : index.a];

        const float r = colour.Red / 31.0f;
        const float g = colour.Green / 31.0f;
        const float b = colour.Blue / 31.0f;

        return tr_colour4
        {
            .Red = static_cast<uint8_t>(std::min(1.0f, r) * 255),
            .Green = static_cast<uint8_t>(std::min(1.0f, g) * 255),
            .Blue = static_cast<uint8_t>(std::min(1.0f, b) * 255)
        };
    }

    uint16_t Level::convert_textile4(uint16_t tile, uint16_t clut_id)
    {
        // Check if we've already converted this tile + clut
        for (auto i = _converted_t16.begin(); i < _converted_t16.end(); ++i)
        {
            if (i->first == tile && i->second == clut_id)
            {
                return static_cast<uint16_t>(std::distance(_converted_t16.begin(), i));
            }
        }
        // If not, create new conversion
        _converted_t16.push_back(std::make_pair(tile, clut_id));

        tr_textile16& tile16 = _textile16.emplace_back();
        if (tile < _textile4.size() && clut_id < _clut.size())
        {
            const tr_textile4& tile4 = _textile4[tile];
            const tr_clut& clut = _clut[clut_id];
            for (int x = 0; x < 256; ++x)
            {
                for (int y = 0; y < 256; ++y)
                {
                    const std::size_t pixel = (y * 256 + x);
                    const tr_colorindex4& index = tile4.Tile[pixel / 2];
                    const tr_rgba5551& colour = clut.Colour[(x % 2) ? index.b : index.a];
                    tile16.Tile[pixel] = (colour.Alpha << 15) | (colour.Red << 10) | (colour.Green << 5) | colour.Blue;
                }
            }
        }
        else
        {
            memset(tile16.Tile, 0, sizeof(tile16.Tile));
        }

        _num_textiles = static_cast<uint32_t>(_textile16.size());
        return static_cast<uint16_t>(_textile16.size() - 1);
    };

    void Level::load(const LoadCallbacks& callbacks)
    {
        // Clear the log before loading the level so we don't keep accumulating memory.
        _log->clear();

        // Load the level from the file.
        _name = trview::filename_without_path(_filename);

        trview::Activity activity(_log, "IO", "Load Level " + _name);

        try
        {
            auto converted = trview::to_utf16(_filename);
            activity.log(std::format("Opening file \"{}\"", _filename));

            auto bytes = _files->load_file(_filename);
            if (!bytes.has_value())
            {
                throw LevelLoadException();
            }

            std::basic_ispanstream<uint8_t> file{ { *bytes } };
            auto file_position = [&]() { return static_cast<uint64_t>(file.tellg()); };

            log_file(activity, file, std::format("Opened file \"{}\"", _filename));

            log_file(activity, file, "Reading version number from file");
            uint32_t raw_version = read<uint32_t>(file);
            _platform_and_version = convert_level_version(raw_version);

            log_file(activity, file, std::format("Version number is {:X} ({}), Platform is {}", raw_version, to_string(get_version()), to_string(platform())));
            if (_platform_and_version.version == LevelVersion::Unknown)
            {
                throw LevelLoadException(std::format("Unknown level version ({})", raw_version));
            }

            if (raw_version == 0x63345254)
            {
                callbacks.on_progress("Decrypting");
                log_file(activity, file, std::format("File is encrypted, decrypting"));
                _decrypter->decrypt(bytes.value());
                file.seekg(0, std::ios::beg);
                raw_version = read<uint32_t>(file);
                _platform_and_version = convert_level_version(raw_version);
                log_file(activity, file, std::format("Version number is {:X} ({})", raw_version, to_string(get_version())));
            }

            if (is_tr5(activity, get_version(), converted))
            {
                _platform_and_version.version = LevelVersion::Tomb5;
                log_file(activity, file, std::format("Version number is {:X} ({})", raw_version, to_string(get_version())));
            }

            if (get_version() >= LevelVersion::Tomb4)
            {
                load_tr4(activity, file, callbacks);
                callbacks.on_progress("Loading complete");
                return;
            }

            if (get_version() > LevelVersion::Tomb1)
            {
                callbacks.on_progress("Reading 8-bit palette");
                log_file(activity, file, "Reading 8-bit palette");
                _palette = read_vector<tr_colour>(file, 256);
                callbacks.on_progress("Reading 16-bit palette");
                log_file(activity, file, "Reading 16-bit palette");
                _palette16 = read_vector<tr_colour4>(file, 256);
            }

            if (_platform_and_version.platform == Platform::PSX)
            {
                if (_platform_and_version.version == LevelVersion::Tomb1)
                {
                    skip(file, 12);
                    uint32_t textile_address = read<uint32_t>(file);
                    file.seekg(textile_address + 8, std::ios::beg);
                }
            }

            callbacks.on_progress("Reading textiles");
            log_file(activity, file, "Reading textiles");
            if (_platform_and_version.platform == Platform::PSX)
            {
                auto at = file.tellg();
                _num_textiles = 13;
                _textile4 = read_vector<tr_textile4>(file, 13);
                _clut = read_vector<tr_clut>(file, 1024);
                log_file(activity, file, std::format("Read {} textile4s and {} clut", _textile4.size(), _clut.size()));
            }
            else
            {
                _num_textiles = read<uint32_t>(file);
                if (get_version() > LevelVersion::Tomb1)
                {
                    callbacks.on_progress(std::format("Skipping {} 8-bit textiles", _num_textiles));
                    log_file(activity, file, std::format("Skipping {} 8-bit textiles", _num_textiles));
                    skip(file, sizeof(tr_textile8) * _num_textiles);
                }
                else
                {
                    callbacks.on_progress(std::format("Reading {} 8-bit textiles", _num_textiles));
                    log_file(activity, file, std::format("Reading {} 8-bit textiles", _num_textiles));
                    _textile8 = read_vector<tr_textile8>(file, _num_textiles);
                }
            }

            if (get_version() > LevelVersion::Tomb1)
            {
                callbacks.on_progress(std::format("Reading {} 16-bit textiles", _num_textiles));
                log_file(activity, file, std::format("Reading {} 16-bit textiles", _num_textiles));
                stream_vector<tr_textile16>(file, _num_textiles, [&](auto&& t) { callbacks.on_textile(convert_textile(t)); });
            }

            load_level_data(activity, file, callbacks);
            callbacks.on_progress("Generating meshes");
            generate_meshes(_mesh_data);
            callbacks.on_progress("Loading complete");
        }
        catch (const LevelEncryptedException&)
        {
            activity.log(trview::Message::Status::Error, "Level is encrypted, aborting");
            throw;
        }
        catch (const std::exception& e)
        {
            activity.log(trview::Message::Status::Error, std::format("Level failed to load: {}", e.what()));
            throw LevelLoadException(e.what());
        }
    }
}
