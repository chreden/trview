#include <gtest/gtest.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/IFont.h>

using namespace trview;
using namespace trview::graphics;

TEST(FontFactory, KeysAreCaseInsensitive)
{
    FontFactory factory;
    ASSERT_THROW(factory.create_font("arial", 8), std::runtime_error);
    factory.store("ARIAL8", std::shared_ptr<DirectX::SpriteFont>());
    auto font = factory.create_font("arial", 8);
    ASSERT_NE(font, nullptr);
}
