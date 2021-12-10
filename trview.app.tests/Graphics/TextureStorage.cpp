#include <gtest/gtest.h>
#include <trview.app/Graphics/TextureStorage.h>
#include <trview.graphics/mocks/IDevice.h>

using namespace trview;
using namespace trview::graphics;
using namespace trview::graphics::mocks;

TEST(TextureStorage, KeysAreCaseInsensitive)
{
    TextureStorage storage(std::make_shared<MockDevice>());
    auto existing_texture = storage.lookup("test_key");
    ASSERT_EQ(existing_texture.name(), std::string());

    Texture store_texture;
    store_texture.set_name("test");
    storage.store("TEST_KEY", store_texture);

    auto texture = storage.lookup("test_key");
    ASSERT_EQ(texture.name(), "test");
}
