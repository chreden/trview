#include "pch.h"
#include "ItemsWindowTests.h"
#include <trview.app/Windows/ItemsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>
#include <trview.tests.common/Messages.h>
#include <trview.app/Messages/Messages.h>

#include <ranges>
#include <format>

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
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::unique_ptr<ItemsWindow> build()
            {
                return std::make_unique<ItemsWindow>(clipboard, messaging);
            }

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }

            test_module& with_messaging(const std::shared_ptr<IMessageSystem>& messaging)
            {
                this->messaging = messaging;
                return *this;
            }
        };

        return test_module{};
    }

    struct ItemsWindowContext
    {
        std::unique_ptr<ItemsWindow> ptr;
        std::vector<std::shared_ptr<IItem>> items;
        std::vector<std::shared_ptr<ITrigger>> triggers;
        std::shared_ptr<MockMessageSystem> messaging;
        std::vector<trview::Message> messages;
    };

    void render(ItemsWindowContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_items_window_tests(ImGuiTestEngine* engine)
{
    test<ItemsWindowContext>(engine, "Items Window", "Add to Route Event Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();

            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            context.items =
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_selected_item(context.items[1]);

            ctx->ItemClick("Items 0/**/Add to Route");

            if (auto found = find_message(context.messages, "add_to_route"))
            {
                auto route_message = messages::read_add_to_route(found.value());
                IM_CHECK_EQ(std::holds_alternative<std::weak_ptr<IItem>>(route_message->element), true);
                IM_CHECK_EQ(std::get<std::weak_ptr<IItem>>(route_message->element).lock(), context.items[1]);
            }
            else
            {
                IM_CHECK_EQ(false, true);
            }
        });

    test<ItemsWindowContext>(engine, "Items Window", "Click Stat Copies",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto clipboard = mock_shared<MockClipboard>();
            EXPECT_CALL(*clipboard, write).Times(1);

            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().with_clipboard(clipboard).build();

            context.items =  { mock_shared<MockItem>() };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_selected_item(context.items[0]);

            ctx->ItemClick("Items 0/**/##itemstats/Position");
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(clipboard.get()), true);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Item Selected Not Raised When Sync Item Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            auto messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(messaging).build();

            std::optional<trview::Message> called_item;
            EXPECT_CALL(*messaging, send_message).Times(0).WillRepeatedly(SaveArg<0>(&called_item));

            context.items =
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };
            context.ptr->set_items({ std::from_range, context.items });

            ctx->ItemUncheck("Items 0/**/Sync");
            ctx->ItemClick("Items 0/**/1##1");

            IM_CHECK_EQ(called_item.has_value(), false);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Item Selected Raised When Sync Item Enabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            auto messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(messaging).build();

            std::optional<trview::Message> called_item;
            EXPECT_CALL(*messaging, send_message).Times(1).WillRepeatedly(SaveArg<0>(&called_item));

            context.items =
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };
            context.ptr->set_items({ std::from_range, context.items });

            ctx->ItemClick("Items 0/**/1##1");

            IM_CHECK_EQ(std::static_pointer_cast<MessageData<std::weak_ptr<IItem>>>(called_item->data)->value.lock(), context.items[1]);

            const auto from_window = context.ptr->selected_item().lock();
            IM_CHECK_EQ(from_window, context.items[1]);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Items List Filtered When Room Set and Track Room Enabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            auto messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(messaging).build();

            std::optional<trview::Message> called_item;
            EXPECT_CALL(*messaging, send_message).Times(1).WillRepeatedly(SaveArg<0>(&called_item));

            auto room_78 = mock_shared<MockRoom>()->with_number(78);

            context.items =
            {
                mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
                mock_shared<MockItem>()->with_number(1)->with_room(room_78)
            };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_current_room(room_78);

            ctx->ItemClick("Items 0/**/Track##track");
            ctx->ItemCheck("/**/Room");
            ctx->KeyPress(ImGuiKey_Escape);
            ctx->ItemClick("Items 0/**/1##1");

            IM_CHECK_EQ(ctx->ItemExists("Items 0/**/0##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("Items 0/**/1##1"), true);
            IM_CHECK_EQ(std::static_pointer_cast<MessageData<std::weak_ptr<IItem>>>(called_item->data)->value.lock(), context.items[1]);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Items List Not Filtered When Room Set and Track Room Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            auto messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(messaging).build();

            std::optional<trview::Message> called_item;
            EXPECT_CALL(*messaging, send_message).Times(1).WillRepeatedly(SaveArg<0>(&called_item));

            auto room_78 = mock_shared<MockRoom>()->with_number(78);

            context.items =
            {
                mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
                mock_shared<MockItem>()->with_number(1)->with_room(room_78)
            };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_current_room(room_78);

            ctx->ItemClick("Items 0/**/0##0");

            IM_CHECK_EQ(std::static_pointer_cast<MessageData<std::weak_ptr<IItem>>>(called_item->data)->value.lock(), context.items[0]);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Items List Populated on Set",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().build();
            context.items =
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };
            context.ptr->set_items({ std::from_range, context.items });

            for (std::size_t i = 0; i < context.items.size(); ++i)
            {
                IM_CHECK_EQ(ctx->ItemExists(std::format("Items 0/**/{0}##{0}", i).c_str()), true);
            }
        });

    test<ItemsWindowContext>(engine, "Items Window", "Items List Updated When Not Filtered",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().build();
            context.items =
            {
                mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55))->with_visible(true),
                mock_shared<MockItem>()->with_number(1)->with_room(mock_shared<MockRoom>()->with_number(78))->with_visible(true)
            };
            context.ptr->set_items({ std::from_range, context.items });

            IM_CHECK_EQ(ctx->ItemExists("Items 0/**/1##1"), true);
            IM_CHECK_EQ(ctx->ItemExists("Items 0/**/##Hide-1"), true);
            IM_CHECK_EQ(ctx->ItemIsChecked("Items 0/**/##Hide-1"), false);

            ON_CALL(*std::static_pointer_cast<MockItem>(context.items[1]), visible).WillByDefault(testing::Return(false));

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemIsChecked("Items 0/**/##Hide-1"), true);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Item Visibility Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().build();

            auto item1 = mock_shared<MockItem>()->with_number(0)->with_visible(true);
            auto item2 = mock_shared<MockItem>()->with_number(1)->with_updating_visible(true);
            EXPECT_CALL(*item2, set_visible(false)).Times(1);

            context.items = { item1, item2 };
            context.ptr->set_items({ std::from_range, context.items });

            ctx->ItemCheck("Items 0/**/##Hide-1");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(context.items[1].get()), true);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Trigger Selected Event Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto trigger = mock_shared<MockTrigger>();
            std::vector<std::shared_ptr<MockItem>> items
            {
                mock_shared<MockItem>()->with_number(0),
                    mock_shared<MockItem>()->with_number(1)->with_triggers({ trigger })
            };
            context.ptr->set_items({ std::from_range, items });
            context.ptr->set_triggers({ trigger });

            ctx->ItemClick("Items 0/**/##1");
            ctx->ItemClick("Items 0/**/0");

            if (auto found = find_message(context.messages, "select_trigger"))
            {
                auto selected_trigger = messages::read_select_trigger(found.value());
                IM_CHECK_EQ(selected_trigger->lock(), trigger);
            }
            else
            {
                IM_CHECK_EQ(false, true);
            }
        });

    test<ItemsWindowContext>(engine, "Items Window", "Triggers Loaded For Item",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().build();

            context.triggers =
            {
                mock_shared<MockTrigger>()->with_number(0),
                mock_shared<MockTrigger>()->with_number(1)
            };
            context.items =
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)->with_triggers({ std::from_range, context.triggers })
            };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_triggers({ std::from_range, context.triggers });

            ctx->ItemClick("Items 0/**/1##1");

            for (std::size_t i = 0; i < context.items.size(); ++i)
            {
                IM_CHECK_EQ(ctx->ItemExists(std::format("Items 0/**/{0}", i).c_str()), true);
            }
        });

}
