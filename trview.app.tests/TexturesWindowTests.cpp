#include <trview.app/Windows/Textures/TexturesWindow.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<TexturesWindow> build()
            {
                return std::make_unique<TexturesWindow>();
            }
        };

        return test_module{};
    }
}

TEST(TexturesWindow, RegularTileUsed)
{
    auto window = register_test_module().build();
    auto storage = mock_shared<MockLevelTextureStorage>();
    ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
    EXPECT_CALL(*storage, texture).Times(0);
    EXPECT_CALL(*storage, opaque_texture).Times(0);
    EXPECT_CALL(*storage, texture(0)).Times(1);

    window->set_texture_storage(storage);

    TestImgui imgui([&]() { window->render(); });
}

TEST(TexturesWindow, OpaqueTileUsed)
{
    auto window = register_test_module().build();

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Textures 0")
        .id(TexturesWindow::Names::transparency_checkbox));

    auto storage = mock_shared<MockLevelTextureStorage>();
    ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
    EXPECT_CALL(*storage, texture).Times(0);
    EXPECT_CALL(*storage, opaque_texture).Times(0);
    EXPECT_CALL(*storage, opaque_texture(0)).Times(1);
    window->set_texture_storage(storage);

    imgui.render();
}

TEST(TexturesWindow, ChangeTile)
{
    auto window = register_test_module().build();

    auto storage = mock_shared<MockLevelTextureStorage>();
    ON_CALL(*storage, num_tiles).WillByDefault(Return(2));
    EXPECT_CALL(*storage, texture).Times(0);
    EXPECT_CALL(*storage, texture(0)).Times(2);
    EXPECT_CALL(*storage, texture(1)).Times(2);
    EXPECT_CALL(*storage, opaque_texture).Times(0);
    window->set_texture_storage(storage);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Textures 0")
        .id(TexturesWindow::Names::tile));
    imgui.enter_text("1");
    imgui.press_key(ImGuiKey_Enter);
}
