#include "gtest/gtest.h"
#include <trview.ui/Image.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(Image, LoadFromJson)
{
    const std::string json = "{ \"type\":\"image\" }";
    auto image = ui::load_from_json(json);
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