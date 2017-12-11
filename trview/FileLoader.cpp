#include "stdafx.h"
#include "FileLoader.h"

#include <fstream>

namespace trview
{
    // Load the contents of a file.
    std::vector<char> load_file(std::wstring filename)
    {
        std::ifstream shaderfile;
        shaderfile.open(filename, std::ios::binary);
        shaderfile.seekg(0, std::ios::end);
        std::size_t length = shaderfile.tellg();
        shaderfile.seekg(0, std::ios::beg);
        std::vector<char> data(length, 0);
        shaderfile.read(&data[0], length);
        shaderfile.close();
        return data;
    }
}