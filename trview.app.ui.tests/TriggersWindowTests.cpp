#include "pch.h"
#include "TriggersWindowTests.h"
#include <trview.app/Windows/TriggersWindow.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.tests.common/Mocks.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<TriggersWindow> build()
            {
                return std::make_unique<TriggersWindow>(clipboard);
            }
        };

        return test_module{};
    }

    struct TriggersWindowContext final
    {
        std::unique_ptr<TriggersWindow> ptr;
        std::vector<std::shared_ptr<MockTrigger>> triggers;
        std::vector<std::shared_ptr<MockItem>> items;

        void render()
        {
            if (ptr)
            {
                ptr->render();
            }
        }
    };
}

void register_triggers_window_tests(ImGuiTestEngine* engine)
{
    test<TriggersWindowContext>(engine, "Triggers Window", "Add to Route Event Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            std::shared_ptr<ITrigger> raised_trigger;
            auto token = context.ptr->on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger.lock(); };

            auto trigger = mock_shared<MockTrigger>();
            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/Add to Route");

            IM_CHECK_EQ(raised_trigger, trigger);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Camera Sink Selected Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            std::shared_ptr<ICameraSink> raised;
            auto token = context.ptr->on_camera_sink_selected += [&raised](const auto& cam) { raised = cam.lock(); };

            auto cam = mock_shared<MockCameraSink>();

            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, camera_sink(100)).WillByDefault(Return(cam));

            auto trigger = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Camera, 100) });
            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/Camera##0");

            IM_CHECK_EQ(raised, cam);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Click Stat Shows Bubble",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_commands({ Command(0, TriggerCommandType::FlipOff, 0) });
            context.triggers = { trigger1 };
            context.ptr->set_triggers({ trigger1 });
            context.ptr->set_selected_trigger(trigger1);

            IM_CHECK_EQ(ctx->ItemExists("/**/##Tooltip_00"), false);
            ctx->ItemClick("/**/Position");
            ctx->Yield(10);
            IM_CHECK_EQ(ctx->ItemExists("/**/##Tooltip_00"), true);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Flipmap Filters All Flip Triggers",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();
            auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_commands({ Command(0, TriggerCommandType::FlipOff, 0) });
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_commands({ Command(0, TriggerCommandType::FlipOn, 0) });
            auto trigger3 = mock_shared<MockTrigger>()->with_number(2)->with_commands({ Command(0, TriggerCommandType::FlipMap, 0) });
            auto trigger4 = mock_shared<MockTrigger>()->with_number(3);
            context.triggers = { trigger1, trigger2, trigger3, trigger4 };
            context.ptr->set_triggers({ trigger1, trigger2, trigger3, trigger4 });

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/2##2"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/3##3"), true);

            ctx->SetRef(ctx->ItemInfo("/**/0##0")->Window->ParentWindow);
            ctx->ComboClick("##commandfilter/Flipmaps");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/2##2"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/3##3"), false);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Item Selected Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            std::shared_ptr<IItem> raised_item;
            auto token = context.ptr->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

            context.items = 
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };

            auto trigger = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Object, 1) });
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_triggers({ trigger });
            context.triggers = { trigger };
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/Item##0");

            auto selected = context.ptr->selected_trigger().lock();
            IM_CHECK_EQ(selected, trigger);
            IM_CHECK_EQ(raised_item, context.items[1]);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Reset Colour",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto trigger = mock_shared<MockTrigger>();
            EXPECT_CALL(*trigger, set_colour(std::optional<Colour>{}));

            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/Reset");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(trigger.get()), true);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Triggers List Filtered By Command",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto trigger1 = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Object, 1) });
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_commands({ Command(0, TriggerCommandType::Camera, 1) });
            std::vector<std::weak_ptr<ITrigger>> triggers{ trigger1, trigger2 };
            context.triggers = { trigger1, trigger2 };
            context.ptr->set_triggers(triggers);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);

            ctx->SetRef(ctx->ItemInfo("/**/0##0")->Window->ParentWindow);
            ctx->ComboClick("##commandfilter/Camera");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Triggers List Not Filtered When Room Set and Track Room Disabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto room_78 = mock_shared<MockRoom>()->with_number(78);
            auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
            context.ptr->set_triggers({ trigger1, trigger2 });
            context.ptr->set_current_room(room_78);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Triggers List Filtered When Room Set and Track Room Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto room_78 = mock_shared<MockRoom>()->with_number(78);
            auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55));
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_room(room_78);
            context.triggers = { trigger1, trigger2 };
            context.ptr->set_triggers({ trigger1, trigger2 });
            context.ptr->set_current_room(room_78);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);

            ctx->ItemClick("/**/Track##track");
            ctx->ItemCheck("/**/Room");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/0##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/1##1"), true);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Trigger Selected Raised When Sync Trigger Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            std::shared_ptr<ITrigger> raised_trigger;
            auto token = context.ptr->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger.lock(); };

            auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
            context.ptr->set_triggers({ trigger1, trigger2 });

            ctx->ItemClick("/**/1##1");

            IM_CHECK_EQ(raised_trigger, trigger2);

            const auto from_window = context.ptr->selected_trigger().lock();
            IM_CHECK_EQ(from_window, trigger2);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Trigger Selected Not Raised When Sync Trigger Disabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            std::shared_ptr<ITrigger> raised_trigger;
            auto token = context.ptr->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger.lock(); };

            auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
            context.ptr->set_triggers({ trigger1, trigger2 });

            ctx->ItemUncheck("/**/Sync");
            ctx->ItemClick("/**/1##1");

            IM_CHECK_EQ(raised_trigger, nullptr);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Trigger Visibility Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            bool raised = false;
            auto token = context.ptr->on_scene_changed += [&raised]() { raised = true; };

            auto trigger = mock_shared<MockTrigger>()->with_visible(true);
            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            EXPECT_CALL(*trigger, set_visible(false)).Times(1);

            ctx->ItemCheck("/**/##hide-0");

            IM_CHECK_EQ(raised, true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(trigger.get()), true);
        });
}