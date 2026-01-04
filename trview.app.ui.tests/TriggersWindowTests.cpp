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
#include <trview.common/Mocks/Messages/IMessageSystem.h>
#include <trview.tests.common/Messages.h>
#include <trview.app/Messages/Messages.h>

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
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::unique_ptr<TriggersWindow> build()
            {
                return std::make_unique<TriggersWindow>(clipboard, messaging);
            }

            test_module& with_messaging(const std::shared_ptr<MockMessageSystem>& messaging)
            {
                this->messaging = messaging;
                return *this;
            }
        };

        return test_module{};
    }

    struct TriggersWindowContext final
    {
        std::unique_ptr<TriggersWindow> ptr;
        std::vector<std::shared_ptr<MockTrigger>> triggers;
        std::vector<std::shared_ptr<MockItem>> items;
        std::shared_ptr<MockMessageSystem> messaging;
        std::vector<trview::Message> messages;

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
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto trigger = mock_shared<MockTrigger>();
            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/Add to Route");

            if (auto found = find_message(context.messages, "add_to_route"))
            {
                auto route_message = messages::read_add_to_route(found.value());
                IM_CHECK_EQ(std::holds_alternative<std::weak_ptr<ITrigger>>(route_message->element), true);
                IM_CHECK_EQ(std::get<std::weak_ptr<ITrigger>>(route_message->element).lock(), trigger);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Camera Sink Selected Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto cam = mock_shared<MockCameraSink>();

            auto level = mock_shared<MockLevel>();
            ON_CALL(*level, camera_sink(100)).WillByDefault(Return(cam));

            auto trigger = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Camera, { 100 }) })->with_level(level);
            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/0##command-0");

            if (const auto found = find_message(context.messages, "select_camera_sink"))
            {
                IM_CHECK_EQ(messages::read_select_camera_sink(found.value())->lock(), cam);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Click Stat Shows Bubble",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_commands({ Command(0, TriggerCommandType::FlipOff, { 0 }) });
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
            auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_commands({ Command(0, TriggerCommandType::FlipOff, { 0 }) });
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_commands({ Command(0, TriggerCommandType::FlipOn, { 0 }) });
            auto trigger3 = mock_shared<MockTrigger>()->with_number(2)->with_commands({ Command(0, TriggerCommandType::FlipMap, { 0 }) });
            auto trigger4 = mock_shared<MockTrigger>()->with_number(3);
            context.triggers = { trigger1, trigger2, trigger3, trigger4 };
            context.ptr->set_triggers({ trigger1, trigger2, trigger3, trigger4 });

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##2"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##3"), true);

            ctx->SetRef(ctx->ItemInfo("/**/##0")->Window->ParentWindow);
            ctx->ComboClick("##commandfilter/Flipmaps");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##2"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##3"), false);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Item Selected Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            context.items = 
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };

            auto trigger = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Object, { 1 }) });
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_triggers({ trigger });
            context.triggers = { trigger };
            context.ptr->set_selected_trigger(trigger);

            ctx->ItemClick("/**/0##command-0");

            auto selected = context.ptr->selected_trigger().lock();
            IM_CHECK_EQ(selected, trigger);
            
            if (const auto found = find_message(context.messages, "select_item"))
            {
                IM_CHECK_EQ(messages::read_select_item(found.value())->lock(), context.items[1]);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
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

            auto trigger1 = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Object, { 1 }) });
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_commands({ Command(0, TriggerCommandType::Camera, { 1 }) });
            std::vector<std::weak_ptr<ITrigger>> triggers{ trigger1, trigger2 };
            context.triggers = { trigger1, trigger2 };
            context.ptr->set_triggers(triggers);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);

            ctx->SetRef(ctx->ItemInfo("/**/##0")->Window->ParentWindow);
            ctx->ComboClick("##commandfilter/Camera");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);
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

            context.ptr->receive_message(trview::Message{ .type = "select_room", .data = std::make_shared<MessageData<std::weak_ptr<IRoom>>>(room_78) });

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);
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
            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);

            ctx->ItemClick("/**/Track##track");
            ctx->ItemCheck("/**/Room");
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Trigger Selected Raised When Sync Trigger Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
            context.ptr->set_triggers({ trigger1, trigger2 });

            ctx->ItemClick("/**/##1");

            if (const auto found = find_message(context.messages, "select_trigger"))
            {
                IM_CHECK_EQ(messages::read_select_trigger(found.value())->lock(), trigger2);
            }
            else
            {
                IM_ERRORF("Message not found");
            }

            const auto from_window = context.ptr->selected_trigger().lock();
            IM_CHECK_EQ(from_window, trigger2);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Trigger Selected Not Raised When Sync Trigger Disabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
            auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
            context.ptr->set_triggers({ trigger1, trigger2 });

            ctx->ItemUncheck("/**/Sync");
            ctx->ItemClick("/**/##1");

            IM_CHECK_EQ(find_message(context.messages, "select_trigger"), std::nullopt);
        });

    test<TriggersWindowContext>(engine, "Triggers Window", "Trigger Visibility Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<TriggersWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<TriggersWindowContext>();
            context.ptr = register_test_module().build();

            auto trigger = mock_shared<MockTrigger>()->with_updating_visible(true);
            context.triggers = { trigger };
            context.ptr->set_triggers({ trigger });
            EXPECT_CALL(*trigger, set_visible(false)).Times(1);

            ctx->ItemCheck("/**/##Hide-0");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(trigger.get()), true);
        });
}