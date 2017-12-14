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
    }

    Level::Level(std::wstring filename)
    {
        // Load the level from the file.
        std::ifstream file;
        file.open(filename.c_str(), std::ios::binary);

        uint32_t version = read<uint32_t>(file);

        file.seekg(768, std::ios::cur); // 8-bit pallete.
        file.seekg(1024, std::ios::cur); // 16 bit palette.

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
            std::vector<char> room_data(NumDataWords * 2);
            if (NumDataWords > 0)
            {
                int16_t vertices = read<int16_t>(file);
                for (int j = 0; j < vertices; ++j)
                {
                    auto vertex = read<tr3_room_vertex>(file);
                }

                int16_t rects = read<int16_t>(file);
                for (int j = 0; j < rects; ++j)
                {
                    auto rect = read<tr_face4>(file);
                }

                int16_t tris = read<int16_t>(file);
                for (int j = 0; j < tris; ++j)
                {
                    auto tri = read<tr_face3>(file);
                }

                int16_t sprites = read<int16_t>(file);
                for (int j = 0; j < sprites; ++j)
                {
                    auto sprite = read<tr_room_sprite>(file);
                }
            }

            uint16_t portals = read<uint16_t>(file);
            for (int j = 0; j < portals; ++j)
            {
                auto portal = read<tr_room_portal>(file);
            }

            uint16_t num_z = read<uint16_t>(file);
            uint16_t num_x = read<uint16_t>(file);
            for (int j = 0; j < num_z * num_x; ++j)
            {
                auto sector = read<tr_room_sector>(file);
            }

            int16_t ambient1 = read<int16_t>(file);
            int16_t ambient2 = read<int16_t>(file);

            uint16_t lights = read<uint16_t>(file);
            for (int j = 0; j < lights; ++j)
            {
                auto light = read<tr3_room_light>(file);
            }

            uint16_t meshes = read<uint16_t>(file);
            for (int j = 0; j < meshes; ++j)
            {
                auto meshe = read<tr3_room_staticmesh>(file);
            }

            int16_t alternate = read<int16_t>(file);
            int16_t flags = read<int16_t>(file);

            uint8_t water_scheme = read<uint8_t>(file);
            uint8_t reverb_info = read<uint8_t>(file);
            uint8_t filler = read<uint8_t>(file);

            _rooms.push_back(room);
        }
    }

    Level::~Level()
    {
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
}