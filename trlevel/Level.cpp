#include "Level.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace trlevel
{
    namespace
    {
        const float PiMul2 = 6.283185307179586476925286766559;
    }

    namespace
    {
        template <typename T>
        T read(std::istream& file)
        {
            T value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            return value;
        }

        template < typename DataType, typename SizeType >
        std::vector<DataType> read_vector(std::istream& file, SizeType size)
        {
            std::vector<DataType> data;
            for (SizeType i = 0; i < size; ++i)
            {
                data.emplace_back(read<DataType>(file));
            }
            return data;
        }

        template < typename SizeType, typename DataType >
        std::vector<DataType> read_vector(std::istream& file)
        {
            auto size = read<SizeType>(file);
            return read_vector<DataType, SizeType>(file, size);
        }
    }

    Level::Level(std::wstring filename)
    {
        // Load the level from the file.
        std::ifstream file;
        file.open(filename.c_str(), std::ios::binary);

        _version = convert_level_version(read<uint32_t>(file));

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

        uint32_t unused = read<uint32_t>(file);

        _num_rooms = read<uint16_t>(file);

        for (uint16_t i = 0; i < _num_rooms; ++i)
        {
            tr3_room room;
            room.info = read<tr_room_info>(file);
            
            uint32_t NumDataWords = read<uint32_t>(file);

            // Read actual room data.
            if (NumDataWords > 0)
            {
                if (_version == LevelVersion::Tomb1)
                {
                    room.data.vertices = convert_vertices(read_vector<int16_t, tr_room_vertex>(file));
                }
                else
                {
                    room.data.vertices = read_vector<int16_t, tr3_room_vertex>(file);
                }
                room.data.rectangles = read_vector<int16_t, tr_face4>(file);
                room.data.triangles = read_vector<int16_t, tr_face3>(file);
                room.data.sprites = read_vector<int16_t, tr_room_sprite>(file);
            }

            room.portals = read_vector<uint16_t, tr_room_portal>(file);

            room.num_z_sectors = read<uint16_t>(file);
            room.num_x_sectors = read<uint16_t>(file);
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);

            int16_t ambient1 = read<int16_t>(file);

            if (_version > LevelVersion::Tomb1)
            {
                int16_t ambient2 = read<int16_t>(file);
            }

            if (get_version() == LevelVersion::Tomb2)
            {
                int16_t lightmode = read<int16_t>(file);
            }

            if (_version == LevelVersion::Tomb1)
            {
                room.lights = convert_lights(read_vector<uint16_t, tr_room_light>(file));
            }
            else
            {
                room.lights = read_vector<uint16_t, tr3_room_light>(file);
            }

            if (_version == LevelVersion::Tomb1)
            {
                room.static_meshes = convert_room_static_meshes(read_vector<uint16_t, tr_room_staticmesh>(file));
            }
            else
            {
                room.static_meshes = read_vector<uint16_t, tr3_room_staticmesh>(file);
            }

            int16_t alternate = read<int16_t>(file);
            int16_t flags = read<int16_t>(file);

            if (get_version() == LevelVersion::Tomb3)
            {
                uint8_t water_scheme = read<uint8_t>(file);
                uint8_t reverb_info = read<uint8_t>(file);
                uint8_t filler = read<uint8_t>(file);
            }

            _rooms.push_back(room);
        }

        _floor_data = read_vector<uint32_t, uint16_t>(file);

        std::vector<uint16_t> mesh_data = read_vector<uint32_t, uint16_t>(file);
        _mesh_pointers = read_vector<uint32_t, uint32_t>(file);
        std::vector<tr_animation> animations = read_vector<uint32_t, tr_animation>(file);
        std::vector<tr_state_change> state_changes = read_vector<uint32_t, tr_state_change>(file);
        std::vector<tr_anim_dispatch> anim_dispatches = read_vector<uint32_t, tr_anim_dispatch>(file);
        std::vector<tr_anim_command> anim_commands = read_vector<uint32_t, tr_anim_command>(file);
        _meshtree = read_vector<uint32_t, uint32_t>(file);
        _frames = read_vector<uint32_t, uint16_t>(file);
        _models = read_vector<uint32_t, tr_model>(file);

        auto static_meshes = read_vector<uint32_t, tr_staticmesh>(file);
        for (const auto& mesh : static_meshes)
        {
            _static_meshes.insert({ mesh.ID, mesh });
        }

        if (get_version() < LevelVersion::Tomb3)
        {
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
        }

        _sprite_textures = read_vector<uint32_t, tr_sprite_texture>(file);
        _sprite_sequences = read_vector<uint32_t, tr_sprite_sequence>(file);

        // If this is Unfinished Business, the palette is here.
        // Need to do something about that, instead of just crashing.

        std::vector<tr_camera> cameras = read_vector<uint32_t, tr_camera>(file);
        std::vector<tr_sound_source> sound_sources = read_vector<uint32_t, tr_sound_source>(file);

        uint32_t num_boxes = 0;
        if (_version == LevelVersion::Tomb1)
        {
            std::vector<tr_box> boxes = read_vector<uint32_t, tr_box>(file);
            num_boxes = boxes.size();
        }
        else
        {
            std::vector<tr2_box> boxes = read_vector<uint32_t, tr2_box>(file);
            num_boxes = boxes.size();
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

        if (get_version() == LevelVersion::Tomb3)
        {
            _object_textures = read_vector<uint32_t, tr_object_texture>(file);
        }

        if (_version == LevelVersion::Tomb1)
        {
            _entities = convert_entities(read_vector<uint32_t, tr_entity>(file));
        }
        else
        {
            _entities = read_vector<uint32_t, tr2_entity>(file);
        }
        std::vector<uint8_t> light_map = read_vector<uint8_t>(file, 32 * 256);

        if (_version == LevelVersion::Tomb1)
        {
            _palette = read_vector<tr_colour>(file, 256);
        }

        std::vector<tr_cinematic_frame> cinematic_frames = read_vector<uint16_t, tr_cinematic_frame>(file);
        std::vector<uint8_t> demo_data = read_vector<uint16_t, uint8_t>(file);

        if (_version == LevelVersion::Tomb1)
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 256);
        }
        else
        {
            std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
        }

        std::vector<tr3_sound_details> sound_details = read_vector<uint32_t, tr3_sound_details>(file);

        if (_version == LevelVersion::Tomb1)
        {
            std::vector<uint8_t> sound_data = read_vector<int32_t, uint8_t>(file);
        }

        std::vector<uint32_t> sample_indices = read_vector<uint32_t, uint32_t>(file);

        generate_meshes(mesh_data);
    }

    Level::~Level()
    {
    }

    void Level::generate_meshes(std::vector<uint16_t> mesh_data)
    {
        // As well as reading the actual mesh data, generate a map of mesh_pointer to 
        // mesh. It seems that a lot of the pointers point to the same mesh.

        std::string data(reinterpret_cast<char*>(&mesh_data[0]), mesh_data.size() * sizeof(uint16_t));
        std::istringstream stream(data, std::ios::binary);
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
            
            mesh.textured_rectangles = read_vector<int16_t, tr_face4>(stream);
            mesh.textured_triangles = read_vector<int16_t, tr_face3>(stream);
            mesh.coloured_rectangles = read_vector<int16_t, tr_face4>(stream);
            mesh.coloured_triangles = read_vector<int16_t, tr_face3>(stream);
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

        if (index < _textile16.size())
        {
            auto textile = _textile16[index];
            std::transform(textile.Tile, 
                textile.Tile + sizeof(textile.Tile) / sizeof(uint16_t),  
                std::back_inserter(results),
                convert_textile16);
        }
        else
        {
            auto textile = _textile8[index];
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

    uint16_t Level::num_rooms() const
    {
        return _num_rooms;
    }

    tr3_room Level::get_room(uint16_t index) const
    {
        return _rooms[index];
    }

    uint32_t Level::num_object_textures() const
    {
        return _object_textures.size();
    }

    tr_object_texture Level::get_object_texture(uint32_t index) const
    {
        return _object_textures[index];
    }

    uint16_t Level::get_floor_data(uint32_t index) const
    {
        return _floor_data[index];
    }

    uint32_t Level::num_entities() const
    {
        return _entities.size();
    }

    tr2_entity Level::get_entity(uint32_t index) const 
    {
        return _entities[index];
    }

    uint32_t Level::num_models() const
    {
        return _models.size();
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
        return _static_meshes.size();
    }

    tr_staticmesh Level::get_static_mesh(uint32_t mesh_id) const
    {
        return _static_meshes.find(mesh_id)->second;
    }

    tr_mesh Level::get_mesh_by_pointer(uint16_t mesh_pointer) const
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

        for (int i = 0; i < mesh_count; ++i)
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
                float angle = (data & 0x03ff) * PiMul2 / 1024.0f;
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

    bool Level::get_sprite_sequence_by_id(uint32_t sprite_sequence_id, tr_sprite_sequence& output) const
    {
        for (const auto& sequence : _sprite_sequences)
        {
            if (sequence.SpriteID == sprite_sequence_id)
            {
                output = sequence;
                return true;
            }
        }
        return false;
    }

    tr_sprite_texture Level::get_sprite_texture(uint32_t index) const
    {
        return _sprite_textures[index];
    }
}
