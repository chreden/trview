#include <trview.app/Graphics/TextureStorage.h>

using namespace trview;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::tests;

TEST(TextureStorage, KeysAreCaseInsensitive)
{
    TextureStorage storage(mock_shared<MockDevice>());
    auto existing_texture = storage.lookup("test_key");
    ASSERT_EQ(existing_texture.name(), std::string());

    Texture store_texture;
    store_texture.set_name("test");
    storage.store("TEST_KEY", store_texture);

    auto texture = storage.lookup("test_key");
    ASSERT_EQ(texture.name(), "test");
}

TEST(TextureStorage, TexturesGenerated)
{
    std::vector<uint32_t> data;
    data.resize(256 * 256, 0x000080ff);

    auto device = mock_shared<MockDevice>();

    std::vector<std::vector<uint32_t>> saved_data;
    EXPECT_CALL(*device, create_texture_2D).Times(2).WillRepeatedly(
        [&](auto&& desc, auto&& data)
        {
            const uint32_t* ptr = reinterpret_cast<const uint32_t*>(data.pSysMem);
            saved_data.push_back({ ptr, ptr + 256 * 256 });
            return nullptr;
        });
    EXPECT_CALL(*device, create_shader_resource_view).Times(2);

    TextureStorage subject(device);

    // 0: TRLE
    ASSERT_EQ(saved_data.size(), 2u);
    ASSERT_EQ(saved_data[0].size(), 65536u);
}
