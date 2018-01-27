#include "Level.h"
#include <fstream>
#include <iostream>

namespace trlevel
{
    namespace
    {
        template <typename T>
        T read(std::ifstream& file)
        {
            T value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            return value;
        }

        template < typename DataType, typename SizeType >
        std::vector<DataType> read_vector(std::ifstream& file, SizeType size)
        {
            std::vector<DataType> data;
            for (SizeType i = 0; i < size; ++i)
            {
                data.emplace_back(read<DataType>(file));
            }
            return data;
        }

        template < typename SizeType, typename DataType >
        std::vector<DataType> read_vector(std::ifstream& file)
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

        uint32_t version = read<uint32_t>(file);

        _palette = read_vector<tr_colour>(file, 256);
        _palette16 = read_vector<tr_colour4>(file, 256);

        _num_textiles = read<uint32_t>(file);
        for (uint32_t i = 0; i < _num_textiles; ++i)
        {
            _textile8.emplace_back(read<tr_textile8>(file));
        }

        for (uint32_t i = 0; i < _num_textiles; ++i)
        {
            _textile16.emplace_back(read<tr_textile16>(file));
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
                room.data.vertices = read_vector<int16_t, tr3_room_vertex>(file);
                room.data.rectangles = read_vector<int16_t, tr_face4>(file);
                room.data.triangles = read_vector<int16_t, tr_face3>(file);
                room.data.sprites = read_vector<int16_t, tr_room_sprite>(file);
            }

            room.portals = read_vector<uint16_t, tr_room_portal>(file);

            room.num_z_sectors = read<uint16_t>(file);
            room.num_x_sectors = read<uint16_t>(file);
            room.sector_list = read_vector<tr_room_sector>(file, room.num_z_sectors * room.num_x_sectors);

            int16_t ambient1 = read<int16_t>(file);
            int16_t ambient2 = read<int16_t>(file);

            room.lights = read_vector<uint16_t, tr3_room_light>(file);
            room.static_meshes = read_vector<uint16_t, tr3_room_staticmesh>(file);

            int16_t alternate = read<int16_t>(file);
            int16_t flags = read<int16_t>(file);

            uint8_t water_scheme = read<uint8_t>(file);
            uint8_t reverb_info = read<uint8_t>(file);
            uint8_t filler = read<uint8_t>(file);

            _rooms.push_back(room);
        }

        _floor_data = read_vector<uint32_t, uint16_t>(file);

        std::vector<uint16_t> mesh_data = read_vector<uint32_t, uint16_t>(file);
        std::vector<uint32_t> mesh_pointers = read_vector < uint32_t, uint32_t>(file);
        std::vector<tr_animation> animations = read_vector<uint32_t, tr_animation>(file);
        std::vector<tr_state_change> state_changes = read_vector<uint32_t, tr_state_change>(file);
        std::vector<tr_anim_dispatch> anim_dispatches = read_vector<uint32_t, tr_anim_dispatch>(file);
        std::vector<tr_anim_command> anim_commands = read_vector<uint32_t, tr_anim_command>(file);
        // std::vector<tr_meshtree_node> mesh_trees = read_vector<uint32_t, tr_meshtree_node>(file);
        std::vector<tr2_meshtree> mesh_trees = read_vector<uint32_t, tr2_meshtree>(file);
        std::vector<uint16_t> frames = read_vector<uint32_t, uint16_t>(file);
        std::vector<tr_model> models = read_vector<uint32_t, tr_model>(file);
        std::vector<tr_staticmesh> static_meshes = read_vector<uint32_t, tr_staticmesh>(file);
        std::vector<tr_sprite_texture> sprite_textures = read_vector<uint32_t, tr_sprite_texture>(file);
        std::vector<tr_sprite_sequence> sprite_sequences = read_vector<uint32_t, tr_sprite_sequence>(file);
        std::vector<tr_camera> cameras = read_vector<uint32_t, tr_camera>(file);
        std::vector<tr_sound_source> sound_sources = read_vector<uint32_t, tr_sound_source>(file);
        std::vector<tr2_box> boxes = read_vector<uint32_t, tr2_box>(file);
        std::vector<uint16_t> overlaps = read_vector<uint32_t, uint16_t>(file);
        std::vector<int16_t> zones = read_vector<int16_t>(file, boxes.size() * 10);
        std::vector<uint16_t> animated_textures = read_vector<uint32_t, uint16_t>(file);
        _object_textures = read_vector<uint32_t, tr_object_texture>(file);
        std::vector<tr2_entity> entities = read_vector<uint32_t, tr2_entity>(file);
        std::vector<uint8_t> light_map = read_vector<uint8_t>(file, 32 * 256);
        std::vector<tr_cinematic_frame> cinematic_frames = read_vector<uint16_t, tr_cinematic_frame>(file);
        std::vector<uint8_t> demo_data = read_vector<uint16_t, uint8_t>(file);
        std::vector<int16_t> sound_map = read_vector<int16_t>(file, 370);
        std::vector<tr3_sound_details> sound_details = read_vector<uint32_t, tr3_sound_details>(file);
        std::vector<uint32_t> sample_indices = read_vector<uint32_t, uint32_t>(file);
    }

    Level::~Level()
    {
    }

    tr_colour Level::get_palette_entry(uint32_t index) const
    {
        return _palette[index];
    }

    tr_colour4 Level::get_palette_entry_16(uint32_t index) const
    {
        return _palette16[index];
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
}