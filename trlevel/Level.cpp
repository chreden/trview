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

        file.seekg(768, std::ios::cur);
        file.seekg(1024, std::ios::cur);

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
        uint16_t num_rooms = read<uint16_t>(file);
    }

    Level::~Level()
    {
    }

    uint32_t Level::num_textiles() const
    {
        return uint32_t();
    }

    tr_textile8 Level::get_textile8(uint32_t index) const
    {
        return tr_textile8();
    }

    tr_textile16 Level::get_textile16(uint32_t index) const
    {
        return tr_textile16();
    }
}