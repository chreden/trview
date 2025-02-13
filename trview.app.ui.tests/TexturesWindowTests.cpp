#include "pch.h"
#include "TexturesWindowTests.h"
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Windows/Textures/TexturesWindow.h>
#include <trview.app/Mocks/Elements/ILevel.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

void register_textures_window_tests(ImGuiTestEngine* engine)
{
    test<TexturesWindow>(engine, "Textures Window", "Change Tile",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TexturesWindow>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<TexturesWindow>();

            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(2));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, texture(0)).Times(AtLeast(1));
            EXPECT_CALL(*storage, texture(1)).Times(0);
            EXPECT_CALL(*storage, opaque_texture).Times(0);

            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, texture_storage).WillByDefault(Return(storage));

            window.add_level(level);

            ctx->Yield();
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(storage.get()), true);

            ON_CALL(*storage, num_tiles).WillByDefault(Return(2));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, texture(0)).Times(AtLeast(1));
            EXPECT_CALL(*storage, texture(1)).Times(AtLeast(1));
            EXPECT_CALL(*storage, opaque_texture).Times(0);

            ctx->ItemInputValue("/**/Tile", "1");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(storage.get()), true);
        });

    test<TexturesWindow>(engine, "Textures Window", "Index Clamped On Set",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TexturesWindow>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<TexturesWindow>();

            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(2));

            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, texture_storage).WillByDefault(Return(storage));
            window.add_level(level);

            ctx->ItemInputValue("/**/Tile", "1");

            ON_CALL(*storage, num_tiles).WillByDefault(Return(1));

            ctx->Yield();

            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("/**/Tile")->ID).c_str(), "0");
        });

    test<TexturesWindow>(engine, "Textures Window", "Opaque Tile Used",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TexturesWindow>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<TexturesWindow>();

            ctx->ItemUncheck("/**/Transparency");

            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, opaque_texture).Times(0);
            EXPECT_CALL(*storage, opaque_texture(0)).Times(1);
            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, texture_storage).WillByDefault(Return(storage));
            window.add_level(level);

            ctx->Yield();

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(storage.get()), true);
        });

    test<TexturesWindow>(engine, "Textures Window", "Regular Tile Used",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TexturesWindow>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<TexturesWindow>();
            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, opaque_texture).Times(0);
            EXPECT_CALL(*storage, texture(0)).Times(1);
            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, texture_storage).WillByDefault(Return(storage));
            window.add_level(level);

            ctx->Yield();

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(storage.get()), true);
        });
}
