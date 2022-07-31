#pragma once

namespace trlevel
{
    struct IDecrypter
    {
        virtual ~IDecrypter() = 0;
        virtual void decrypt(std::vector<uint8_t>& bytes) const = 0;
    };
}
