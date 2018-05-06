#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace trview
{
    // Load the contents of a file.
    std::vector<uint8_t> load_file(std::wstring filename);
}