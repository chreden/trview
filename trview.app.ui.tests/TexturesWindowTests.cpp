#include "pch.h"
#include "TexturesWindowTests.h"
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Windows/Textures/TexturesWindow.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::unique_ptr<TexturesWindow> build()
            {
                return std::make_unique<TexturesWindow>(messaging);
            }

            test_module& with_messaging(const std::shared_ptr<IMessageSystem>& messaging)
            {
                this->messaging = messaging;
                return *this;
            }
        };

        return test_module{};
    }

    struct TexturesWindowContext
    {
        std::unique_ptr<TexturesWindow> ptr;
        std::shared_ptr<MockMessageSystem> messaging;
    };

    void render(TexturesWindowContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_textures_window_tests(ImGuiTestEngine* engine)
{
    test<TexturesWindowContext>(engine, "Textures Window", "Change Tile",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<TexturesWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TexturesWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();

            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(2));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, texture(0)).Times(AtLeast(1));
            EXPECT_CALL(*storage, texture(1)).Times(0);
            EXPECT_CALL(*storage, opaque_texture).Times(0);
            context.ptr->set_texture_storage(storage);

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

    test<TexturesWindowContext>(engine, "Textures Window", "Index Clamped On Set",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<TexturesWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TexturesWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();

            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(2));
            context.ptr->set_texture_storage(storage);

            ctx->ItemInputValue("/**/Tile", "1");

            ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
            context.ptr->set_texture_storage(storage);

            ctx->Yield();

            IM_CHECK_STR_EQ(ItemText(ctx, ctx->ItemInfo("/**/Tile")->ID).c_str(), "0");
        });

    test<TexturesWindowContext>(engine, "Textures Window", "Opaque Tile Used",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<TexturesWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TexturesWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();

            ctx->ItemUncheck("/**/Transparency");

            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, opaque_texture).Times(0);
            EXPECT_CALL(*storage, opaque_texture(0)).Times(1);
            context.ptr->set_texture_storage(storage);

            ctx->Yield();

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(storage.get()), true);
        });

    test<TexturesWindowContext>(engine, "Textures Window", "Regular Tile Used",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<TexturesWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TexturesWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            auto storage = mock_shared<MockLevelTextureStorage>();
            ON_CALL(*storage, num_tiles).WillByDefault(Return(1));
            EXPECT_CALL(*storage, texture).Times(0);
            EXPECT_CALL(*storage, opaque_texture).Times(0);
            EXPECT_CALL(*storage, texture(0)).Times(1);
            context.ptr->set_texture_storage(storage);

            ctx->Yield();

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(storage.get()), true);
        });
}
