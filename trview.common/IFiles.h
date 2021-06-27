#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace trview
{
    struct IFiles
    {
        virtual ~IFiles() = 0;
        virtual std::vector<uint8_t> load_file(const std::string& filename) const = 0;
        virtual void save_file(const std::string& filename, const std::vector<uint8_t>& bytes) const = 0;
    };
}
