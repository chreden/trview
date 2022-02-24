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
