#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace trview
{
    // Load the contents of a file.
    std::vector<char> load_file(std::wstring filename);
}