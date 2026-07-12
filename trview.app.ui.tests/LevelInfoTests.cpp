#include "pch.h"
#include "LevelInfoTests.h"

#include <trview.app/UI/LevelInfo.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <trview.app/Mocks/Elements/ILevelNameLookup.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.tests.common/Mocks.h>
#include <trview.common/Messages/Message.h>

using namespace testing;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<ILevelNameLookup> level_name_lookup{ mock_shared<MockLevelNameLookup>() };
            std::shared_ptr<MockTextureStorage> texture_storage{ mock_shared<MockTextureStorage>() };

            std::unique_ptr<LevelInfo> build()
            {
                return std::make_unique<LevelInfo>(*texture_storage, level_name_lookup);
            }

            test_module& with_level_name_lookup(const std::shared_ptr<ILevelNameLookup>& level_name_lookup)
            {
                this->level_name_lookup = level_name_lookup;
                return *this;
            }
        };

        return test_module{};
    }

    struct LevelInfoContext
    {
        std::unique_ptr<LevelInfo> ptr;
    };

    void render(LevelInfoContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_level_info_tests(ImGuiTestEngine* engine)
{
    test<LevelInfoContext>(engine, "Level Info", "Name Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<LevelInfoContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto lookup = mock_shared<MockLevelNameLookup>();
            ON_CALL(*lookup, lookup(A<const std::weak_ptr<ILevel>&>())).WillByDefault(testing::Return(ILevelNameLookup::Name{ .name = "test" }));

            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, name).WillByDefault(Return("Test"));

            auto& context = ctx->GetVars<LevelInfoContext>();
            context.ptr = register_test_module().with_level_name_lookup(lookup).build();
            context.ptr->receive_message(trview::Message{ .type = "open_level", .data = std::make_shared<MessageData<std::weak_ptr<ILevel>>>(level) });

            ctx->Yield();

            IM_CHECK_EQ(RenderedText(ctx, ImGui::GetCurrentWindow()->ID), "test (Test)");
        });
}