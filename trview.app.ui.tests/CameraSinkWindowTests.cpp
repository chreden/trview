#include "pch.h"
#include "CameraSinkWindowTests.h"
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Windows/CameraSink/CameraSinkWindow.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.tests.common/Mocks.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>
#include <trview.app/Messages/Messages.h>
#include <trview.tests.common/Messages.h>

using namespace testing;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    namespace
    {
        auto register_test_module()
        {
            struct test_module
            {
                std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };
                std::shared_ptr<ICamera> camera{ mock_shared<MockCamera>() };
                std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

                std::unique_ptr<CameraSinkWindow> build()
                {
                    return std::make_unique<CameraSinkWindow>(clipboard, camera, messaging);
                }

                test_module& with_messaging(const std::shared_ptr<IMessageSystem>& messaging)
                {
                    this->messaging = messaging;
                    return *this;
                }
            };

            return test_module{};
        }
    }

    struct CameraSinkWindowContext final
    {
        std::unique_ptr<CameraSinkWindow> ptr;
        std::vector<std::shared_ptr<MockCameraSink>> camera_sinks;
        std::vector<std::shared_ptr<MockTrigger>> triggers;
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

void register_camera_sink_window_tests(ImGuiTestEngine* engine)
{
    test<CameraSinkWindowContext>(engine, "Camera/Sink Window", "List Filtered When Room Set and Track Room Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraSinkWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<CameraSinkWindowContext>();
            context.ptr = register_test_module().build();

            auto room_78 = mock_shared<MockRoom>()->with_number(78);
            auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(56));
            auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1)->with_room(room_78);
            context.ptr->set_camera_sinks({ camera_sink1, camera_sink2 });
            context.ptr->set_current_room(room_78);
            context.camera_sinks = { camera_sink1, camera_sink2 };

            ctx->ItemClick("/**/Track##track");
            ctx->ItemCheck("/**/Room");

            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), false);;
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);;
        });

    test<CameraSinkWindowContext>(engine, "Camera/Sink Window", "Selected Not Raised When Sync Off",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraSinkWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<CameraSinkWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0);
            auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1);
            context.camera_sinks = { camera_sink1, camera_sink2 };
            context.ptr->set_camera_sinks({ camera_sink1, camera_sink2 });

            ctx->ItemUncheck("/**/Sync");
            ctx->ItemClick("/**/##1");

            ASSERT_EQ(true, testing::Mock::VerifyAndClearExpectations(context.messaging.get()));
        });

    test<CameraSinkWindowContext>(engine, "Camera/Sink Window", "Selected Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraSinkWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<CameraSinkWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0);
            auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1);
            context.camera_sinks = { camera_sink1, camera_sink2 };
            context.ptr->set_camera_sinks({ camera_sink1, camera_sink2 });

            ctx->ItemClick("/**/##1");

            if (auto found = find_message(context.messages, "select_camera_sink"))
            {
                auto selected = messages::read_select_camera_sink(found.value());
                IM_CHECK_EQ(selected->lock(), camera_sink2);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
        });

    test<CameraSinkWindowContext>(engine, "Camera/Sink Window", "Trigger Selected Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraSinkWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<CameraSinkWindowContext>();
            context.messaging = mock_shared<MockMessageSystem>();
            context.ptr = register_test_module().with_messaging(context.messaging).build();
            ON_CALL(*context.messaging, send_message).WillByDefault([&](auto&& message) { context.messages.push_back(message); });

            auto trigger1 = mock_shared<MockTrigger>();
            auto trigger2 = mock_shared<MockTrigger>();
            ON_CALL(*trigger2, number).WillByDefault(testing::Return(1));
            context.triggers = { trigger1, trigger2 };
            auto camera_sink = mock_shared<MockCameraSink>() ->with_number(0)->with_triggers({ trigger1, trigger2 });
            context.camera_sinks = { camera_sink };
            context.ptr->set_camera_sinks({ camera_sink });
            context.ptr->set_selected_camera_sink(camera_sink);

            ctx->ItemClick("/**/1");

            if (auto found = find_message(context.messages, "select_trigger"))
            {
                auto selected = messages::read_select_trigger(found.value());
                IM_CHECK_EQ(selected->lock(), trigger2);
            }
            else
            {
                IM_ERRORF("Message not found");
            }
        });

    test<CameraSinkWindowContext>(engine, "Camera/Sink Window", "Type Changed Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraSinkWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<CameraSinkWindowContext>();
            context.ptr = register_test_module().build();

            auto camera_sink = mock_shared<MockCameraSink>()->with_number(0)->with_type(ICameraSink::Type::Camera);
            EXPECT_CALL(*camera_sink, set_type(ICameraSink::Type::Sink)).Times(1);

            context.camera_sinks = { camera_sink };
            context.ptr->set_camera_sinks({ camera_sink });
            context.ptr->set_selected_camera_sink(camera_sink);

            ctx->SetRef("/Camera\\/Sink 0\\/Details_CA3F050C");
            ctx->ComboClick("Type/Sink##type");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(camera_sink.get()), true);
        });

    test<CameraSinkWindowContext>(engine, "Camera/Sink Window", "Visibility Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<CameraSinkWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<CameraSinkWindowContext>();
            context.ptr = register_test_module().build();

            auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0);
            auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1)->with_updating_visible(true);
            ON_CALL(*camera_sink2, visible).WillByDefault(testing::Return(true));
            EXPECT_CALL(*camera_sink2, set_visible(false)).Times(1);
            context.camera_sinks = { camera_sink1, camera_sink2 };
            context.ptr->set_camera_sinks({ camera_sink1, camera_sink2 });

            ctx->ItemClick("/**/##Hide-1");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(camera_sink2.get()), true);
        });
}
