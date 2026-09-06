export module trlevel:IHasher;

import std;
import std.compat;

namespace trlevel
{
    export struct IHasher
    {
        virtual ~IHasher() = 0;
        virtual std::string hash(const std::vector<uint8_t>& data) const = 0;
    };
}
