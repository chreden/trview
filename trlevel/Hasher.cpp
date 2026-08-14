#include "Hasher.h"
#include <bcrypt.h>
#include <format>
#include <ranges>
#include <versionhelpers.h>

namespace trlevel
{
    IHasher::~IHasher()
    {
    }

    Hasher::Hasher()
    {
    }

    Hasher::~Hasher()
    {
        BCryptDestroyHash(_hash);
    }

    void Hasher::create_hash() const
    {
        const bool reusable = IsWindows8OrGreater();
        if (reusable && _hash != nullptr)
        {
            return;
        }

        BCryptDestroyHash(_hash);
        _hash = nullptr;

        const bool use_pseudo_handle = IsWindows10OrGreater();

        BCRYPT_ALG_HANDLE algorithm = BCRYPT_SHA256_ALG_HANDLE;
        if (!use_pseudo_handle)
        {
            BCryptOpenAlgorithmProvider(&algorithm, L"SHA256", NULL, 0);
        }
        
        DWORD object_length = 0;
        DWORD data_length = 0;
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PBYTE>(&object_length), sizeof(DWORD), &data_length, 0);
        hash_object.resize(object_length, 0);

        DWORD hash_length = 0;
        BCryptGetProperty(algorithm, BCRYPT_HASH_LENGTH, (PBYTE)&hash_length, sizeof(DWORD), &data_length, 0);
        hash_buffer.resize(hash_length, 0);

        BCryptCreateHash(algorithm, &_hash, &hash_object[0], object_length, nullptr, 0, reusable ? BCRYPT_HASH_REUSABLE_FLAG : 0);

        if (!use_pseudo_handle)
        {
            BCryptCloseAlgorithmProvider(&algorithm, 0);
        }
    }

    std::string Hasher::hash(const std::vector<uint8_t>& data) const
    {
        create_hash();
        BCryptHashData(_hash, const_cast<uint8_t*>(&data[0]), static_cast<ULONG>(data.size()), 0);
        BCryptFinishHash(_hash, const_cast<uint8_t*>(&hash_buffer[0]), static_cast<ULONG>(hash_buffer.size()), 0);
        return hash_buffer
            | std::views::transform([](uint8_t b) { return std::format("{:02X}", b); })
            | std::views::join
            | std::ranges::to<std::string>();
    }
}
