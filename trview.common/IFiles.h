#pragma once

#include <string>
#include <cstdint>

namespace trview
{
    struct IFiles
    {
        virtual ~IFiles() = 0;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const = 0;
    };
}
