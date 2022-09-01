#pragma once

#include "IDecrypter.h"

namespace trlevel
{
    class Decrypter : public IDecrypter
    {
    public:
        virtual ~Decrypter() = default;
        virtual void decrypt(std::vector<uint8_t>& bytes) const;
        virtual void encrypt(std::vector<uint8_t>& bytes) const;
    };
}
