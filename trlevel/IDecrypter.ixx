export module trlevel:IDecrypter;

import std;
import std.compat;

namespace trlevel
{
    export struct IDecrypter
    {
        virtual ~IDecrypter() = 0;
        virtual void decrypt(std::vector<uint8_t>& bytes) const = 0;
    };
}
