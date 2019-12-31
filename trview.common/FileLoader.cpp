#include "stdafx.h"
#include "FileLoader.h"

#include <fstream>

namespace trview
{
    // Load the contents of a file.
    std::vector<uint8_t> load_file(std::wstring filename)
    {
        std::ifstream ifs (filename, std::ios::binary | std::ios::ate);
        std::streampos length = ifs.tellg(); // already at end of file

        if (!ifs || !ifs.good())
            throw new std::runtime_error("file could not be opened"); 
        else if (length == (std::streampos)-1)
            throw new std::runtime_error("-1 returned from tellg(), read error likely");

        std::vector<uint8_t> data (static_cast<uint32_t>(length));

        ifs.seekg(0, std::ios::beg); 
        ifs.read(reinterpret_cast<char*>(&data[0]), length);

        return data; 
    }
}