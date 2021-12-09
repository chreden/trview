#include <gmock/gmock.h>
#include "gtest/gtest.h"
#include <trview.ui/Image.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(Image, LoadFromJson)
{
    const std::string json = "{ \"type\":\"image\" }";
    auto image = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(image, nullptr);
    ASSERT_NE(dynamic_cast<Image*>(image.get()), nullptr);
}

TEST(Image, Texture)
{
    Image image{ Size() };

    graphics::Texture texture;
    texture.set_name("Test");

    image.set_texture(texture);
    ASSERT_EQ(image.texture().name(), "Test");
}
