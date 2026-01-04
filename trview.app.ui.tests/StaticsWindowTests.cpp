#include "pch.h"
#include "StaticsWindowTests.h"
#include <trview.app/Mocks/Elements/IStaticMesh.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Windows/Statics/StaticsWindow.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>
#include <trview.tests.common/Messages.h>
#include <trview.app/Messages/Messages.h>

#include <format>
#include <ranges>

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

            std::unique_ptr<StaticsWindow> build()
            {
                return std::make_unique<StaticsWindow>(clipboard, messaging);
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

    struct StaticsWindowContext
    {
        std::unique_ptr<StaticsWindow> ptr;
        std::vector<std::shared_ptr<MockStaticMesh>> statics;
        std::shared_ptr<MockMessageSystem> messaging;
        std::vector<trview::Message> messages;
    };

    void render(StaticsWindowContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_statics_window_tests(ImGuiTestEngine* engine)
{
    test<StaticsWindowContext>(engine, "Statics Window", "Statics List Filtered When Room Set and Track Room Enabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<StaticsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<StaticsWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto room_78 = mock_shared<MockRoom>()->with_number(78);

            context.statics =
            {
                mock_shared<MockStaticMesh>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
                mock_shared<MockStaticMesh>()->with_number(1)->with_room(room_78)
            };
            context.ptr->set_statics({ std::from_range, context.statics });
            context.ptr->set_current_room(room_78);

            ctx->ItemClick("Statics 0/**/Track##track");
            ctx->ItemCheck("/**/Room");
            ctx->KeyPress(ImGuiKey_Escape);
            ctx->ItemClick("Statics 0/**/##1");

            IM_CHECK_EQ(ctx->ItemExists("Statics 0/**/##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("Statics 0/**/##1"), true);
            if (const auto found = find_message(context.messages, "select_static_mesh"))
            {
                IM_CHECK_EQ(messages::read_select_static_mesh(found.value())->lock(), context.statics[1]);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
        });

    test<StaticsWindowContext>(engine, "Statics Window", "Statics List Not Filtered When Room Set and Track Room Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<StaticsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<StaticsWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto room_78 = mock_shared<MockRoom>()->with_number(78);

            context.statics =
            {
                mock_shared<MockStaticMesh>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
                mock_shared<MockStaticMesh>()->with_number(1)->with_room(room_78)
            };
            context.ptr->set_statics({ std::from_range, context.statics });
            context.ptr->set_current_room(room_78);

            ctx->ItemClick("Statics 0/**/##0");

            if (const auto found = find_message(context.messages, "select_static_mesh"))
            {
                IM_CHECK_EQ(messages::read_select_static_mesh(found.value())->lock(), context.statics[0]);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
        });

    test<StaticsWindowContext>(engine, "Statics Window", "Static Selected Not Raised When Sync Static Disabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<StaticsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<StaticsWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            context.statics =
            {
                mock_shared<MockStaticMesh>()->with_number(0),
                mock_shared<MockStaticMesh>()->with_number(1)
            };
            context.ptr->set_statics({ std::from_range, context.statics });

            ctx->ItemUncheck("Statics 0/**/Sync");
            ctx->ItemClick("Statics 0/**/##1");

            IM_CHECK_EQ(find_message(context.messages, "select_static_mesh"), std::nullopt);
        });

    test<StaticsWindowContext>(engine, "Statics Window", "Static Selected Raised When Sync Static Enabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<StaticsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<StaticsWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            context.statics =
            {
                mock_shared<MockStaticMesh>()->with_number(0),
                mock_shared<MockStaticMesh>()->with_number(1)
            };
            context.ptr->set_statics({ std::from_range, context.statics });

            ctx->ItemUncheck("Statics 0/**/Sync");
            ctx->ItemClick("Statics 0/**/##1");

            IM_CHECK_EQ(find_message(context.messages, "select_static_mesh"), std::nullopt);

            const auto from_window = context.ptr->selected_static().lock();
            IM_CHECK_EQ(from_window, context.statics[1]);
        });

    test<StaticsWindowContext>(engine, "Statics Window", "Static Visibility Changed",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<StaticsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<StaticsWindowContext>();
            context.ptr = register_test_module().build();
            
            bool static1_visible = true;
            auto static1 = mock_shared<MockStaticMesh>()->with_number(1);
            ON_CALL(*static1, visible).WillByDefault([&] { return static1_visible; });
            EXPECT_CALL(*static1, set_visible(false)).Times(1).WillRepeatedly([&](bool v) { static1_visible = v; });

            context.statics = { mock_shared<MockStaticMesh>()->with_number(0)->with_visible(true), static1 };
            context.ptr->set_statics({ std::from_range, context.statics });

            ctx->ItemCheck("Statics 0/**/##Hide-1");
        });
}
