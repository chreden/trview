#include "pch.h"
#include "ViewOptionsTests.h"
#include <format>

#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.tests.common/Mocks.h>

#include <trview.app/Mocks/Windows/IWindows.h>
#include <trview.app/Mocks/Windows/IWindow.h>

#include <trview.app/Filters/Filters.h>
#include <trview.common/Messages/Message.h>
#include <trview.app/Messages/Messages.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IWindows> windows;

            std::unique_ptr<ViewOptions> build()
            {
                return std::make_unique<ViewOptions>(windows);
            }

            test_module& with_windows(const std::shared_ptr<IWindows>& windows)
            {
                this->windows = windows;
                return *this;
            }
        };

        return test_module{};
    }

    struct ViewOptionsContext
    {
        std::unique_ptr<ViewOptions> ptr;
    };

    void render(ViewOptionsContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_view_options_tests(ImGuiTestEngine* engine)
{
    test<ViewOptionsContext>(engine, "View Options", "Bounds Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Bounds");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::show_bounding_boxes);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Bounds Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Bounds"), false);

            context.ptr->set_toggle(IViewer::Options::show_bounding_boxes, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Bounds"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Camera/Sink Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Camera\\/Sink");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::camera_sinks);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Camera/Sink Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Camera\\/Sink"), false);

            context.ptr->set_toggle(IViewer::Options::camera_sinks, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Camera\\/Sink"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Depth Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Depth");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::depth_enabled);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Depth Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Depth"), false);

            context.ptr->set_toggle(IViewer::Options::depth_enabled, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Depth"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Depth Value Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, int32_t>> clicked;
            auto token = context.ptr->on_scalar_changed += [&](const std::string& name, int32_t value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemClick("flags/##DepthValue/+");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::depth);
            IM_CHECK_EQ(std::get<1>(clicked.value()), 1);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Checkbox Enabled",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            ctx->SetRef("View Options");
            IM_CHECK_EQ((ctx->ItemInfo("flags/Flip")->InFlags & ImGuiItemFlags_Disabled) != 0, true);
            context.ptr->set_flip_enabled(true);
            ctx->Yield();
            IM_CHECK_EQ((ctx->ItemInfo("flags/Flip")->InFlags & ImGuiItemFlags_Disabled) != 0, false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Checkbox Hidden with Alternate Groups",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_use_alternate_groups(false);

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemExists("flags/Flip"), true);
            context.ptr->set_use_alternate_groups(true);
            ctx->Engine->InfoTasks.clear_delete();
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("flags/Flip"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_flip_enabled(true);

            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Flip");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::flip);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_flip_enabled(true);

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Flip"), false);

            context.ptr->set_toggle(IViewer::Options::flip, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Flip"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Checkbox Filter",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            trview::Message message;
            auto window = mock_shared<MockWindow>();
            EXPECT_CALL(*window, receive_message).WillOnce(testing::SaveArg<0>(&message)).Times(1);
            auto windows = mock_shared<MockWindows>();
            EXPECT_CALL(*windows, create).WillOnce(testing::Return(window));

            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().with_windows(windows).build();
            context.ptr->set_flip_enabled(true);

            ctx->SetRef("View Options");
            ctx->ItemClick("flags/Flip", ImGuiMouseButton_Right);
            ctx->ItemOpen("/**/Filter");
            ctx->ItemClick("/**/##Menu_00/New Window");

            IM_CHECK_EQ(true, testing::Mock::VerifyAndClearExpectations(windows.get()));
            IM_CHECK_EQ(true, testing::Mock::VerifyAndClearExpectations(window.get()));

            const std::vector<Filters<IRoom>::Filter> expected{ {.key = "Alternate", .compare = CompareOp::Exists, .op = Op::And } };
            IM_CHECK_EQ(expected, std::static_pointer_cast<MessageData<std::vector<Filters<IRoom>::Filter>>>(message.data)->value);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Flags Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_use_alternate_groups(true);
            context.ptr->set_alternate_groups({ 1, 3, 5 });

            std::unordered_map<uint32_t, bool> raised;
            auto token = context.ptr->on_alternate_group += [&](uint32_t group, bool state)
            {
                raised[group] = state;
            };

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip");

            IM_CHECK_EQ(raised.size(), 1);
            IM_CHECK_EQ(raised[3], true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Flags Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_use_alternate_groups(true);
            context.ptr->set_alternate_groups({ 1, 3, 5 });
            context.ptr->set_alternate_group(3, true);

            std::unordered_map<uint32_t, bool> raised;
            auto token = context.ptr->on_alternate_group += [&](uint32_t group, bool state)
            {
                raised[group] = state;
            };

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip");
            IM_CHECK_EQ(raised.size(), 1);
            IM_CHECK_EQ(raised[3], false);

            ctx->Yield();
            context.ptr->set_alternate_group(3, true);
            raised.clear();

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip");
            IM_CHECK_EQ(raised.size(), 1);
            IM_CHECK_EQ(raised[3], false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Flip Flags Filter",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            trview::Message message;
            auto window = mock_shared<MockWindow>();
            EXPECT_CALL(*window, receive_message).WillOnce(testing::SaveArg<0>(&message)).Times(1);
            auto windows = mock_shared<MockWindows>();
            EXPECT_CALL(*windows, create).WillOnce(testing::Return(window));

            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().with_windows(windows).build();
            context.ptr->set_use_alternate_groups(true);
            context.ptr->set_alternate_groups({ 1, 3, 5 });

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip", ImGuiMouseButton_Right);
            ctx->ItemOpen("/**/Filter");
            ctx->ItemClick("/**/##Menu_00/New Window");

            IM_CHECK_EQ(true, testing::Mock::VerifyAndClearExpectations(windows.get()));
            IM_CHECK_EQ(true, testing::Mock::VerifyAndClearExpectations(window.get()));

            const std::vector<Filters<IRoom>::Filter> expected{ {.key = "Alternate Group", .compare = CompareOp::Equal, .value = "3", .op = Op::And } };
            IM_CHECK_EQ(expected, std::static_pointer_cast<MessageData<std::vector<Filters<IRoom>::Filter>>>(message.data)->value);
        });

    test<ViewOptionsContext>(engine, "View Options", "Hidden Geometry Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Geometry");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::geometry);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Hidden Geometry Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Geometry"), false);

            context.ptr->set_toggle(IViewer::Options::geometry, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Geometry"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Highlight Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Highlight");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::highlight);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Highlight Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Highlight"), false);

            context.ptr->set_toggle(IViewer::Options::highlight, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Highlight"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Items Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Items");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::items);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Items Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Items"), true);

            context.ptr->set_toggle(IViewer::Options::items, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Items"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Lighting Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Lighting");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::lighting);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Lighting Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Lighting"), true);

            context.ptr->set_toggle(IViewer::Options::lighting, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Lighting"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Notes Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Notes");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::notes);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Notes Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Notes"), true);

            context.ptr->set_toggle(IViewer::Options::notes, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Notes"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Rooms Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Rooms");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::rooms);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Rooms Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Rooms"), true);

            context.ptr->set_toggle(IViewer::Options::rooms, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Rooms"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Sounds Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
                {
                    clicked = { name, value };
                };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Sounds");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::sound_sources);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Sounds Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Sounds"), false);

            context.ptr->set_toggle(IViewer::Options::sound_sources, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Sounds"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Triggers Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Triggers");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::triggers);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Triggers Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Triggers"), true);

            context.ptr->set_toggle(IViewer::Options::triggers, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Triggers"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Water Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Water");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::water);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Water Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Water"), true);

            context.ptr->set_toggle(IViewer::Options::water, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Water"), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Wireframe Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Wireframe");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::wireframe);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Wireframe Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Wireframe"), false);

            context.ptr->set_toggle(IViewer::Options::wireframe, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Wireframe"), true);
        });

    test<ViewOptionsContext>(engine, "View Options", "Animation Checkbox Toggle",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = context.ptr->on_toggle_changed += [&](const std::string& name, bool value)
                {
                    clicked = { name, value };
                };

            ctx->SetRef("View Options");
            ctx->ItemUncheck("flags/Animation");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::animation);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptionsContext>(engine, "View Options", "Animation Checkbox Updated",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ViewOptionsContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ViewOptionsContext>();
            context.ptr = register_test_module().build();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Animation"), true);

            context.ptr->set_toggle(IViewer::Options::animation, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Wireframe"), false);
        });
}
