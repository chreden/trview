#include <trlevel/Decrypter.h>
#include <trview.common/Resources.h>
#include "resource.h"
#include <fstream>

using namespace trlevel;

namespace
{
    std::vector<uint8_t> get_resource(int id)
    {
        auto resource = trview::get_resource_memory(id, L"FILE");
        return std::vector<uint8_t>(resource.data, resource.data + resource.size);
    }
}

TEST(Decrypter, DecryptsFile)
{
    auto encrypted = get_resource(IDR_ENCRYPTED_LAKE);
    auto expected = get_resource(IDR_ORIGINAL_LAKE);

    Decrypter decrypter;
    decrypter.decrypt(encrypted);

    std::ofstream out;
    out.open("test.tr4", std::ios::out | std::ios::binary);
    out.write(reinterpret_cast<char*>(&encrypted[0]), encrypted.size());
    

    ASSERT_EQ(encrypted, expected);
}

TEST(Decrypter, EncryptsFile)
{
    auto original = get_resource(IDR_ORIGINAL_LAKE);
    auto expected = get_resource(IDR_ENCRYPTED_LAKE);

    Decrypter decrypter;
    decrypter.encrypt(original);

    std::ofstream out;
    out.open("enc.tr4", std::ios::out | std::ios::binary);
    out.write(reinterpret_cast<char*>(&original[0]), original.size());

    for (int x = 0; x < original.size(); ++x)
    {
        if (original[x] != expected[x])
        {
            __asm int 3;
        }
    }

    ASSERT_EQ(original, expected);
}
