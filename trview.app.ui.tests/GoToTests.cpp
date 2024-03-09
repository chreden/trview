#include "pch.h"
#include "GoToTests.h"

#include <trview.app/UI/GoTo.h>

using namespace trview;

void register_go_to_tests(ImGuiTestEngine* engine)
{
    test<GoTo>(engine, "Go To", "On Selected Not Raised When Cancelled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<GoTo>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& go_to = ctx->GetVars<GoTo>();
            go_to.toggle_visible();

            std::optional<GoTo::GoToItem> raised;
            auto token = go_to.on_selected += [&](auto value)
            {
                raised = value;
            };

            ctx->ItemClick("/**/##gotoentry");
            ctx->KeyPress(ImGuiKey_Escape);

            IM_CHECK_EQ(raised.has_value(), false);
            IM_CHECK_EQ(go_to.visible(), false);
        });

    test<GoTo>(engine, "Go To", "On Selected Raised Number",
        [](ImGuiTestContext* ctx) { ctx->GetVars<GoTo>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& go_to = ctx->GetVars<GoTo>();

            go_to.toggle_visible();
            go_to.set_items({ {.number = 10, .name = "Item Ten" } });
            std::optional<GoTo::GoToItem> raised;
            auto token = go_to.on_selected += [&](auto value)
            {
                raised = value;
            };

            ctx->ItemInputValue("/**/##gotoentry", "10");
            ctx->ItemNavActivate("/**/Item 10 - Item Ten");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value().number, 10u);
        });

    test<GoTo>(engine, "Go To", "On Selected Raised Text",
        [](ImGuiTestContext* ctx) { ctx->GetVars<GoTo>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& go_to = ctx->GetVars<GoTo>();

            go_to.toggle_visible();
            go_to.set_items({ {.number = 10, .name = "Item Ten" } });
            std::optional<GoTo::GoToItem> raised;
            auto token = go_to.on_selected += [&](auto value)
            {
                raised = value;
            };

            ctx->ItemInputValue("/**/##gotoentry", "Ten");
            ctx->ItemNavActivate("/**/Item 10 - Item Ten");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value().number, 10u);
        });
}