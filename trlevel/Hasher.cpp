#include "Hasher.h"
#include <bcrypt.h>
#include <format>
#include <ranges>

namespace trlevel
{
    IHasher::~IHasher()
    {
    }

    Hasher::Hasher()
    {
        DWORD object_length = 0;
        DWORD data_length = 0;
        BCryptGetProperty(BCRYPT_SHA256_ALG_HANDLE, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PBYTE>(&object_length), sizeof(DWORD), &data_length, 0);
        hash_object.resize(object_length, 0);

        DWORD hash_length = 0;
        BCryptGetProperty(BCRYPT_SHA256_ALG_HANDLE, BCRYPT_HASH_LENGTH, (PBYTE)&hash_length, sizeof(DWORD), &data_length, 0);
        hash_buffer.resize(hash_length, 0);

        BCryptCreateHash(BCRYPT_SHA256_ALG_HANDLE, &_hash, &hash_object[0], object_length, nullptr, 0, BCRYPT_HASH_REUSABLE_FLAG);
    }

    Hasher::~Hasher()
    {
        BCryptDestroyHash(_hash);
    }

    std::string Hasher::hash(const std::vector<uint8_t>& data) const
    {
        BCryptHashData(_hash, const_cast<uint8_t*>(&data[0]), static_cast<ULONG>(data.size()), 0);
        BCryptFinishHash(_hash, const_cast<uint8_t*>(&hash_buffer[0]), static_cast<ULONG>(hash_buffer.size()), 0);
        return hash_buffer
            | std::views::transform([](uint8_t b) { return std::format("{:02X}", b); })
            | std::views::join
            | std::ranges::to<std::string>();
    }
}
