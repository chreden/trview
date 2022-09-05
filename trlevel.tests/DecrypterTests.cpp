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

    ASSERT_EQ(encrypted, expected);
}

TEST(Decrypter, EncryptsFile)
{
    auto original = get_resource(IDR_ORIGINAL_LAKE);
    auto expected = get_resource(IDR_ENCRYPTED_LAKE);

    Decrypter decrypter;
    decrypter.encrypt(original);

    ASSERT_EQ(original, expected);
}
