#include "pch.h"
#include "LevelInfoTests.h"

#include <trview.app/UI/LevelInfo.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>

using namespace testing;
using namespace trview;
using namespace trview::mocks;

void register_level_info_tests(ImGuiTestEngine* engine)
{
    test<MockWrapper<LevelInfo>>(engine, "Level Info", "Name Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<LevelInfo>>()); },
        [](ImGuiTestContext* ctx)
        {
            NiceMock<MockTextureStorage> texture_storage;
            auto& controls = ctx->GetVars<MockWrapper<LevelInfo>>();
            controls.ptr = std::make_unique<LevelInfo>(texture_storage);
            controls.ptr->set_level("test");

            ctx->Yield();
            IM_CHECK_EQ(RenderedText(ctx, ctx->ItemInfo("LevelInfo")->ID), "test");
        });

    test<MockWrapper<LevelInfo>>(engine, "Level Info", "On Toggle Settings Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<MockWrapper<LevelInfo>>()); },
        [](ImGuiTestContext* ctx)
        {
            NiceMock<MockTextureStorage> texture_storage;
            auto& controls = ctx->GetVars<MockWrapper<LevelInfo>>();
            controls.ptr = std::make_unique<LevelInfo>(texture_storage);

            bool raised = false;
            auto token = controls.ptr->on_toggle_settings += [&]()
            {
                raised = true;
            };

            ctx->ItemClick("LevelInfo/Settings");
            IM_CHECK_EQ(raised, true);
        });
}