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
        void create_hash() const;

        mutable BCRYPT_HASH_HANDLE _hash{ nullptr };
        mutable std::vector<uint8_t> hash_object;
        mutable std::vector<uint8_t> hash_buffer;
    };
}
