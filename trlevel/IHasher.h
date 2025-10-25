#pragma once

#include <string>
#include <vector>

namespace trlevel
{
    struct IHasher
    {
        virtual ~IHasher() = 0;
        virtual std::string hash(const std::vector<uint8_t>& data) const = 0;
    };
}
