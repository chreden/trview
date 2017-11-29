#include "trlevel.h"
#include <fstream>
#include <iostream>

namespace trlevel
{
    template <typename T>
    T read(std::ifstream& file)
    {
        T value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        return value;
    }
    
    struct tr_colour
    {
        uint8_t Red;
        uint8_t Green;
        uint8_t Blue;
    };

    void load_level()
    {
        // Load the level from the file.
        std::ifstream file;
        file.open("C:\\GOG Games\\Tomb Raider 1+2+3\\Tomb Raider 3\\data\\jungle.TR2", std::ios::binary);

        uint32_t version = read<uint32_t>(file);

        file.seekg(768, std::ios::cur);
        file.seekg(1024, std::ios::cur);

        uint32_t num_textiles = read<uint32_t>(file);

        file.seekg(65536 * num_textiles, std::ios::cur);
        file.seekg(131072 * num_textiles, std::ios::cur);

        uint32_t unused = read<uint32_t>(file);
        uint16_t num_rooms = read<uint16_t>(file);
    }
}