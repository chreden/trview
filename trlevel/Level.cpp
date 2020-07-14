#include "Level.h"
#include "LevelLoadException.h"

namespace trlevel
{
    namespace
    {
        const float PiMul2 = 6.283185307179586476925286766559f;
        const int16_t Lara = 0;
        const int16_t LaraSkinTR3 = 315;
        const int16_t LaraSkinPostTR3 = 8;
    }

    namespace
    {
        template <typename T>
        T read(std::istream& file)
        {
            T value;
            read<T>(file, value);
            return value;
        }

        template < typename T >
        void read(std::istream& file, T& value)
        {
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
        }

        template < typename DataType, typename SizeType >
        std::vector<DataType> read_vector(std::istream& file, SizeType size)
        {
            std::vector<DataType> data(size);
            for (SizeType i = 0; i < size; ++i)
            {
                read<DataType>(file, data[i]);
            }
            return data;
        }

        template < typename SizeType, typename DataType >
        std::vector<DataType> read_vector(std::istream& file)
        {
            auto size = read<SizeType>(file);
            return read_vector<DataType, SizeType>(file, size);
        }

        std::vector<uint8_t> read_compressed(std::istream& file)
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
        std::vector<DataType> read_vector_compressed(std::istream& file, uint32_t elements)
        {
            auto uncompressed_data = read_compressed(file);
            std::string data(reinterpret_cast<char*>(&uncompressed_data[0]), uncompressed_data.size());
            std::istringstream data_stream(data, std::ios::binary);
            data_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            return read_vector<DataType>(data_stream, elements);
        }

        bool is_tr5(LevelVersion version, const std::wstring& filename)
        {
            if (version != LevelVersion::Tomb4)
            {
                return false;
            }

            std::wstring transformed;
            std::transform(filename.begin(), filename.end(), std::back_inserter(transformed), towupper);
            return transformed.find(L".TRC") != filename.npos;
        }

        void skip(std::istream& file, uint32_t size)
        {
            file.seekg(size, std::ios::cur);
        }

        void skip_xela(std::istream& file)
        {
            skip(file, 4);
        }

        void load_tr1_4_room(std::istream& file, tr3_room& room, LevelVersion version)
        {
            room.info = convert_room_info(read<tr1_4_room_info>(file));

            uint32_t NumDataWords = read<uint32_t>(file);

            // Read actual room data.
            if (NumDataWords > 0)
            {
                if (version == LevelVersion::Tomb1)
                {
                    room.data.vertices = convert_vertices(read_vector<int16_t, tr_room_vertex>(file));
                }
                else
                {
                    room.data.vertices = read_vector<int16_t, tr3_room_vertex>(file);
                }
                room.data.rectangles = convert_rectangles(read_vector<int16_t, tr_face4>(file));
                room.data.triangles = convert_triangles(read_vector<int16_t, tr_face3>(file));
                room.data.sprites = read_vector<int16_t, tr_room_sprite>(file);
            }

            room.portals = read_vector<uint16_t, tr_room_portal>(file);

            room.num_z_sectors = read<uint16_t>(file);
            room.num_x_sectors = read<uint16_t>(file);
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);

            if (version == LevelVersion::Tomb4)
            {
                room.room_colour = read<uint32_t>(file);
            }
            else
            {
                room.ambient_intensity_1 = read<int16_t>(file);

                if (version > LevelVersion::Tomb1)
                {
                    room.ambient_intensity_2 = read<int16_t>(file);
                }
            }

            if (version == LevelVersion::Tomb2)
            {
                room.light_mode = read<int16_t>(file);
            }

            if (version == LevelVersion::Tomb1)
            {
                room.lights = convert_lights(read_vector<uint16_t, tr_room_light>(file));
            }
            else if (version == LevelVersion::Tomb4)
            {
                auto lights = read_vector<uint16_t, tr4_room_light>(file);
            }
            else
            {
                room.lights = read_vector<uint16_t, tr3_room_light>(file);
            }

            if (version == LevelVersion::Tomb1)
            {
                room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh>(file));
            }
            else
            {
                room.static_meshes = read_vector<uint16_t, tr3_room_staticmesh>(file);
            }

            room.alternate_room = read<int16_t>(file);
            room.flags = read<int16_t>(file);

            if (version >= LevelVersion::Tomb3)
            {
                room.water_scheme = read<uint8_t>(file);
                room.reverb_info = read<uint8_t>(file);
                room.alternate_group = read<uint8_t>(file);
            }
        }

        void load_tr5_room(std::istream& file, tr3_room& room)
        {
            skip_xela(file);
            uint32_t room_data_size = read<uint32_t>(file);
            const uint32_t room_start = static_cast<uint32_t>(file.tellg());
            const uint32_t room_end = room_start + room_data_size;

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

            // The offsets start measuring from this position, after all the header information.
            const uint32_t data_start = static_cast<uint32_t>(file.tellg());

            // Discard lights as they are not currently used:
            skip(file, sizeof(tr5_room_light) * header.num_lights);

            file.seekg(data_start + header.start_sd_offset, std::ios::beg);
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);
            room.portals = read_vector<uint16_t, tr_room_portal>(file);

            // Separator
            skip(file, 2);

            file.seekg(data_start + header.end_portal_offset, std::ios::beg);
            room.static_meshes = read_vector<tr3_room_staticmesh>(file, header.num_static_meshes);

            file.seekg(data_start + header.layer_offset, std::ios::beg);
            auto layers = read_vector<tr5_room_layer>(file, header.num_layers);

            file.seekg(data_start + header.poly_offset, std::ios::beg);
            uint16_t vertex_offset = 0;
            for (const auto& layer : layers)
            {
                auto rects = read_vector<tr4_mesh_face4>(file, layer.num_rectangles);
                for (auto& rect : rects)
                {
                    for (auto& v : rect.vertices)
                    {
                        v += vertex_offset;
                    }
                }
                std::copy(rects.begin(), rects.end(), std::back_inserter(room.data.rectangles));

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

            file.seekg(data_start + header.vertices_offset, std::ios::beg);
            for (const auto& layer : layers)
            {
                auto verts = convert_vertices(read_vector<tr5_room_vertex>(file, layer.num_vertices));
                std::copy(verts.begin(), verts.end(), std::back_inserter(room.data.vertices));
            }

            file.seekg(room_end, std::ios::beg);
        }
    }

    Level::Level(const std::string& filename)
    {
        // Load the level from the file.
        try
        {
            // Convert the filename to UTF-16
            auto converted = trview::to_utf16(filename);

            std::ifstream file;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            file.open(converted.c_str(), std::ios::binary);

            _version = convert_level_version(read<uint32_t>(file));
            if (is_tr5(_version, converted))
            {
                _version = LevelVersion::Tomb5;
            }

            if (_version >= LevelVersion::Tomb4)
            {
                load_tr4(file);
                return;
            }

            if (_version > LevelVersion::Tomb1)
            {
                _palette = read_vector<tr_colour>(file, 256);
                _palette16 = read_vector<tr_colour4>(file, 256);
            }

            _num_textiles = read<uint32_t>(file);
            for (uint32_t i = 0; i < _num_textiles; ++i)
            {
                _textile8.emplace_back(read<tr_textile8>(file));
            }

            if (_version > LevelVersion::Tomb1)
            {
                for (uint32_t i = 0; i < _num_textiles; ++i)
                {
                    _textile16.emplace_back(read<tr_textile16>(file));
                }
            }

            load_level_data(file);

            generate_meshes(_mesh_data);
        }
        catch(const std::exception&)
        {
            throw LevelLoadException();
        }
    }

    Level::~Level()
    {
    }

    void Level::generate_meshes(const std::vector<uint16_t>& mesh_data)
    {
        // As well as reading the actual mesh data, generate a map of mesh_pointer to 
        // mesh. It seems that a lot of the pointers point to the same mesh.

        std::string data(reinterpret_cast<const char*>(&mesh_data[0]), mesh_data.size() * sizeof(uint16_t));
        std::istringstream stream(data, std::ios::binary);
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

            if (_version < LevelVersion::Tomb4)
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

    uint32_t Level::num_textiles() const
    {
        return _num_textiles;
    }

    tr_textile8 Level::get_textile8(uint32_t index) const
    {
        return _textile8[index];
    }

    tr_textile16 Level::get_textile16(uint32_t index) const
    {
        return _textile16[index];
    }

    std::vector<uint32_t> Level::get_textile(uint32_t index) const
    {
        std::vector<uint32_t> results;
        results.reserve(256 * 256);

        if (index < _textile32.size())
        {
            const auto& textile = _textile32[index];
            std::transform(textile.Tile,
                textile.Tile + sizeof(textile.Tile) / sizeof(uint32_t),
                std::back_inserter(results),
                convert_textile32);
        }
        else if (index < _textile16.size())
        {
            const auto& textile = _textile16[index];
            std::transform(textile.Tile, 
                textile.Tile + sizeof(textile.Tile) / sizeof(uint16_t),  
                std::back_inserter(results),
                convert_textile16);
        }
        else
        {
            const auto& textile = _textile8[index];
            std::transform(textile.Tile,
                textile.Tile + sizeof(textile.Tile) / sizeof(uint8_t),
                std::back_inserter(results),
                [&](uint8_t entry_index)
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
            });
        }

        return results;
    }

    uint32_t Level::num_rooms() const
    {
        return _rooms.size();
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
        return _floor_data.size();
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

    tr_staticmesh Level::get_static_mesh(uint32_t mesh_id) const
    {
        return _static_meshes.find(mesh_id)->second;
    }

    uint32_t Level::num_mesh_pointers() const
    {
        return _mesh_pointers.size();
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
        if (_version == LevelVersion::Tomb1)
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
            if (_version == LevelVersion::Tomb1)
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
                if (_version >= LevelVersion::Tomb4)
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
        return _version;
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

    tr_sprite_texture Level::get_sprite_texture(uint32_t index) const
    {
        return _sprite_textures[index];
    }

    void Level::load_tr4(std::ifstream& file)
    {
        uint16_t num_room_textiles = read<uint16_t>(file);
        uint16_t num_obj_textiles = read<uint16_t>(file);
        uint16_t num_bump_textiles = read<uint16_t>(file);
        _num_textiles = num_room_textiles + num_obj_textiles + num_bump_textiles;

        _textile32 = read_vector_compressed<tr_textile32>(file, _num_textiles);
        _textile16 = read_vector_compressed<tr_textile16>(file, _num_textiles);
        auto textile32_misc = read_vector_compressed<tr_textile32>(file, 2);

        if (_version == LevelVersion::Tomb5)
        {
            _lara_type = read<uint16_t>(file);
            _weather_type = read<uint16_t>(file);
            file.seekg(28, std::ios::cur);
        }

        if (_version == LevelVersion::Tomb4)
        {
            std::vector<uint8_t> level_data = read_compressed(file);
            std::string data(reinterpret_cast<char*>(&level_data[0]), level_data.size());
            std::istringstream data_stream(data, std::ios::binary);
            load_level_data(data_stream);
        }
        else
        {
            // Skip size of uncompressed and compressed level data as they are
            // unused in TR5.
            skip(file, 8);
            load_level_data(file);
        }

        if (_version == LevelVersion::Tomb5)
        {
            skip(file, 6);
        }

        uint32_t num_sound_samples = read<uint32_t>(file);
        std::vector<tr4_sample> sound_samples(num_sound_samples);
        for (uint32_t i = 0; i < num_sound_samples; ++i)
        {
            sound_samples[i].sound_data = read_compressed(file);
        }

        generate_meshes(_mesh_data);
    }

    void Level::load_level_data(std::istream& file)
    {
        // Read unused value.
        read<uint32_t>(file);

        uint32_t num_rooms = 0;
        if (_version == LevelVersion::Tomb5)
        {
            num_rooms = read<uint32_t>(file);
        }
        else
        {
            num_rooms = read<uint16_t>(file);
        }

        for (auto i = 0u; i < num_rooms; ++i)
        {
            tr3_room room;
            if (_version == LevelVersion::Tomb5)
            {
                load_tr5_room(file, room);
            }
            else
            {
                load_tr1_4_room(file, room, _version);
            }
            _rooms.push_back(room);
        }

        _floor_data = read_vector<uint32_t, uint16_t>(file);

        _mesh_data = read_vector<uint32_t, uint16_t>(file);
        _mesh_pointers = read_vector<uint32_t, uint32_t>(file);
        if (_version >= LevelVersion::Tomb4)
        {
            auto animations = read_vector<uint32_t, tr4_animation>(file);
        }
        else
        {
            std::vector<tr_animation> animations = read_vector<uint32_t, tr_animation>(file);
        }
        std::vector<tr_state_change> state_changes = read_vector<uint32_t, tr_state_change>(file);
        std::vector<tr_anim_dispatch> anim_dispatches = read_vector<uint32_t, tr_anim_dispatch>(file);
        std::vector<tr_anim_command> anim_commands = read_vector<uint32_t, tr_anim_command>(file);
        _meshtree = read_vector<uint32_t, uint32_t>(file);
        _frames = read_vector<uint32_t, uint16_t>(file);

        if (_version < LevelVersion::Tomb5)
        {
            _models = read_vector<uint32_t, tr_model>(file);
        }
        else
        {
            _models = convert_models(read_vector<uint32_t, tr5_model>(file));
        }

        auto static_meshes = read_vector<uint32_t, tr_staticmesh>(file);
        for (const auto& mesh : static_meshes)
        {
            _static_meshes.insert({ mesh.ID, mesh });
        }

        if (get_version() < LevelVersion::Tomb3)
        {
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
        }

        if (_version >= LevelVersion::Tomb4)
        {
            // Skip past the 'SPR' marker.
            file.seekg(3, std::ios::cur);
            if (_version == LevelVersion::Tomb5)
            {
                skip(file, 1);
            }
        }

        _sprite_textures = read_vector<uint32_t, tr_sprite_texture>(file);
        _sprite_sequences = read_vector<uint32_t, tr_sprite_sequence>(file);

        // If this is Unfinished Business, the palette is here.
        // Need to do something about that, instead of just crashing.

        std::vector<tr_camera> cameras = read_vector<uint32_t, tr_camera>(file);

        if (_version >= LevelVersion::Tomb4)
        {
            std::vector<tr4_flyby_camera> flyby_cameras = read_vector<uint32_t, tr4_flyby_camera>(file);
        }

        std::vector<tr_sound_source> sound_sources = read_vector<uint32_t, tr_sound_source>(file);

        uint32_t num_boxes = 0;
        if (_version == LevelVersion::Tomb1)
        {
            std::vector<tr_box> boxes = read_vector<uint32_t, tr_box>(file);
            num_boxes = static_cast<uint32_t>(boxes.size());
        }
        else
        {
            std::vector<tr2_box> boxes = read_vector<uint32_t, tr2_box>(file);
            num_boxes = static_cast<uint32_t>(boxes.size());
        }
        std::vector<uint16_t> overlaps = read_vector<uint32_t, uint16_t>(file);

        if (_version == LevelVersion::Tomb1)
        {
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 6);
        }
        else
        {
            std::vector<int16_t> zones = read_vector<int16_t>(file, num_boxes * 10);
        }
        std::vector<uint16_t> animated_textures = read_vector<uint32_t, uint16_t>(file);

        if (_version >= LevelVersion::Tomb4)
        {
            // Animated textures uv count - not yet used:
            skip(file, 1);

            file.seekg(3, std::ios::cur);
            if (_version == LevelVersion::Tomb5)
            {
                skip(file, 1);
            }
        }

        if (get_version() == LevelVersion::Tomb3)
        {
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
        }
        if (get_version() == LevelVersion::Tomb4)
        {
            _object_textures = convert_object_textures(read_vector<uint32_t, tr4_object_texture>(file));
        }
        else if (get_version() == LevelVersion::Tomb5)
        {
            _object_textures = convert_object_textures(read_vector<uint32_t, tr5_object_texture>(file));
        }

        if (_version == LevelVersion::Tomb1)
        {
            _entities = convert_entities(read_vector<uint32_t, tr_entity>(file));
        }
        else
        {
            // TR4 entity is in here, OCB is not set but goes into intensity2 (convert later).
            _entities = read_vector<uint32_t, tr2_entity>(file);
        }

        if (_version < LevelVersion::Tomb4)
        {
            std::vector<uint8_t> light_map = read_vector<uint8_t>(file, 32 * 256);
        }

        if (_version == LevelVersion::Tomb1)
        {
            _palette = read_vector<tr_colour>(file, 256);
        }

        if (_version >= LevelVersion::Tomb4)
        {
            std::vector<tr4_ai_object> ai_objects = read_vector<uint32_t, tr4_ai_object>(file);
        }

        if (_version < LevelVersion::Tomb4)
        {
            std::vector<tr_cinematic_frame> cinematic_frames = read_vector<uint16_t, tr_cinematic_frame>(file);
        }

        std::vector<uint8_t> demo_data = read_vector<uint16_t, uint8_t>(file);

        if (_version == LevelVersion::Tomb1)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 256);
        }
        else if (_version < LevelVersion::Tomb4)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
        }
        else if (_version == LevelVersion::Tomb4)
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

        std::vector<tr3_sound_details> sound_details = read_vector<uint32_t, tr3_sound_details>(file);

        if (_version == LevelVersion::Tomb1)
        {
            std::vector<uint8_t> sound_data = read_vector<int32_t, uint8_t>(file);
        }

        std::vector<uint32_t> sample_indices = read_vector<uint32_t, uint32_t>(file);
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
        if (type != 0 || _version < LevelVersion::Tomb3)
        {
            return type;
        }

        if (_version > trlevel::LevelVersion::Tomb3)
        {
            return LaraSkinPostTR3;
        }
        return LaraSkinTR3;
    }
}
