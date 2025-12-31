#include "pch.h"
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/UI/ContextMenu.h>
#include <trview.tests.common/Mocks.h>

#include "ContextMenuTests.h"

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IWindows> windows;

            std::unique_ptr<ContextMenu> build()
            {
                return std::make_unique<ContextMenu>(windows);
            }

            test_module& with_windows(const std::shared_ptr<IWindows>& windows)
            {
                this->windows = windows;
                return *this;
            }
        };

        return test_module{};
    }

    struct ContextMenuContext
    {
        std::unique_ptr<ContextMenu> menu;
    };

    void render(ContextMenuContext& context)
    {
        if (context.menu)
        {
            context.menu->render();
        }
    }
}

void register_context_menu_tests(ImGuiTestEngine* engine)
{
    test<ContextMenuContext>(engine, "Context Menu", "Add Mid Waypoint Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_mid_waypoint_enabled(true);

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            IM_CHECK_EQ((ctx->ItemInfo("/**/Add Mid Waypoint")->InFlags & ImGuiItemFlags_Disabled) != 0, false);

            context.menu->set_mid_waypoint_enabled(false);
            ctx->Yield();

            IM_CHECK_EQ((ctx->ItemInfo("/**/Add Mid Waypoint")->InFlags & ImGuiItemFlags_Disabled) != 0, true);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Add Mid Waypoint Not Raised When Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_mid_waypoint_enabled(false);

            bool raised = false;
            auto token = context.menu->on_add_mid_waypoint += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Add Mid Waypoint");

            ASSERT_FALSE(raised);
            ASSERT_TRUE(context.menu->visible());
        });

    test<ContextMenuContext>(engine, "Context Menu", "Add Mid Waypoint Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_mid_waypoint_enabled(true);

            bool raised = false;
            auto token = context.menu->on_add_mid_waypoint += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Add Mid Waypoint");

            IM_CHECK_EQ(raised, true);
            IM_CHECK_EQ(context.menu->visible(), false);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Add Waypoint Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);

            bool raised = false;
            auto token = context.menu->on_add_waypoint += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Add Waypoint");

            IM_CHECK_EQ(raised, true);
            IM_CHECK_EQ(context.menu->visible(), false);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Copy Number",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);

            std::optional<trview::IContextMenu::CopyType> raised;
            auto token = context.menu->on_copy += [&raised](auto type) { raised = type; };

            ctx->ItemOpen("/**/Copy");
            ctx->ItemClick("/##Menu_00/Room\\/Object Number");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), trview::IContextMenu::CopyType::Number);
            IM_CHECK_EQ(context.menu->visible(), false);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Copy Position",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);

            std::optional<trview::IContextMenu::CopyType> raised;
            auto token = context.menu->on_copy += [&raised](auto type) { raised = type; };

            ctx->ItemOpen("/**/Copy");
            ctx->ItemClick("/**/Position");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), trview::IContextMenu::CopyType::Position);
            IM_CHECK_EQ(context.menu->visible(), false);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Hide Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_hide_enabled(true);

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            IM_CHECK_EQ((ctx->ItemInfo("/**/Hide")->InFlags & ImGuiItemFlags_Disabled) != 0, false);

            context.menu->set_hide_enabled(false);
            ctx->Yield();

            IM_CHECK_EQ((ctx->ItemInfo("/**/Hide")->InFlags & ImGuiItemFlags_Disabled) != 0, true);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Hide Not Raised When Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build(); 
            context.menu->set_visible(true);
            context.menu->set_hide_enabled(false);

            bool raised = false;
            auto token = context.menu->on_hide += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Hide");

            ASSERT_FALSE(raised);
            ASSERT_TRUE(context.menu->visible());
        });

    test<ContextMenuContext>(engine, "Context Menu", "Hide Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_hide_enabled(true);

            bool raised = false;
            auto token = context.menu->on_hide += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Hide");

            ASSERT_TRUE(raised);
            ASSERT_FALSE(context.menu->visible());
        });

    test<ContextMenuContext>(engine, "Context Menu", "Orbit Here Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);

            bool raised = false;
            auto token = context.menu->on_orbit_here += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Orbit Here");

            ASSERT_TRUE(raised);
            ASSERT_FALSE(context.menu->visible());
        });

    test<ContextMenuContext>(engine, "Context Menu", "Remove Waypoint Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_remove_enabled(true);

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            IM_CHECK_EQ((ctx->ItemInfo("/**/Remove Waypoint")->InFlags& ImGuiItemFlags_Disabled) != 0, false);

            context.menu->set_remove_enabled(false);
            ctx->Yield();

            IM_CHECK_EQ((ctx->ItemInfo("/**/Remove Waypoint")->InFlags& ImGuiItemFlags_Disabled) != 0, true);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Remove Waypoint Not Raised When Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_remove_enabled(false);

            bool raised = false;
            auto token = context.menu->on_remove_waypoint += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Remove Waypoint");

            ASSERT_FALSE(raised);
            ASSERT_TRUE(context.menu->visible());
        });

    test<ContextMenuContext>(engine, "Context Menu", "Remove Waypoint Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_remove_enabled(true);

            bool raised = false;
            auto token = context.menu->on_remove_waypoint += [&raised]() { raised = true; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemClick("/**/Remove Waypoint");

            ASSERT_TRUE(raised);
            ASSERT_FALSE(context.menu->visible());
        });

    test<ContextMenuContext>(engine, "Context Menu", "Trigger References",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);

            auto trigger = mock_shared<MockTrigger>();
            ON_CALL(*trigger, number).WillByDefault(testing::Return(100));
            ON_CALL(*trigger, type).WillByDefault(testing::Return(TriggerType::Trigger));

            context.menu->set_triggered_by({ trigger });

            std::optional<std::weak_ptr<ITrigger>> raised;
            auto token = context.menu->on_trigger_selected += [&raised](auto type) { raised = type; };

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            ctx->ItemOpen("/**/Trigger References");
            ctx->ItemClick("/##Menu_00/Trigger 100");

            ASSERT_TRUE(raised);
            ASSERT_FALSE(context.menu->visible());
            ASSERT_EQ(raised.value().lock(), trigger);
        });

    test<ContextMenuContext>(engine, "Context Menu", "Filters Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ContextMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ContextMenuContext>();
            context.menu = register_test_module().build();
            context.menu->set_visible(true);
            context.menu->set_tile_filter_enabled(true);

            ctx->MouseClickOnVoid(ImGuiMouseButton_Right);
            IM_CHECK_EQ((ctx->ItemInfo("/**/Filter")->InFlags & ImGuiItemFlags_Disabled) != 0, false);

            context.menu->set_tile_filter_enabled(false);
            ctx->Yield();

            IM_CHECK_EQ((ctx->ItemInfo("/**/Filter")->InFlags & ImGuiItemFlags_Disabled) != 0, true);
        });
}
