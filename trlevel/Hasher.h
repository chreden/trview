#pragma once

#include "IHasher.h"

namespace trlevel
{
    class Hasher final : public IHasher
    {
    public:
        Hasher();
        virtual ~Hasher();
        std::string hash(const std::vector<uint8_t>& data) const;
    private:
        BCRYPT_HASH_HANDLE _hash{ nullptr };
        std::vector<uint8_t> hash_object;
        std::vector<uint8_t> hash_buffer;
    };
}
