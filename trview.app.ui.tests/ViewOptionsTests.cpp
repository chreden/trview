#include <windows.h>
#undef Yield

#include "ViewOptionsTests.h"
#include <imgui_te_context.h>
#include <imgui_te_engine.h>
#include <imgui_te_internal.h>
#include <format>

#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Windows/IViewer.h>

namespace
{
    template <typename T>
    void test(
        ImGuiTestEngine* engine,
        const char* category,
        const char* name,
        ImFuncPtr(ImGuiTestGuiFunc) gui_func,
        ImFuncPtr(ImGuiTestTestFunc) test_func)
    {
        auto t = IM_REGISTER_TEST(engine, category, name);
        t->SetVarsDataType<T>();
        t->GuiFunc = gui_func;
        t->TestFunc = test_func;
    }

    void checkbox_test(ImGuiTestEngine* engine, const char* name)
    {

    }
}


void register_view_options_tests(ImGuiTestEngine* engine)
{
    using namespace trview;

    test<ViewOptions>(engine, "View Options", "Bounds Checkbox Toggle",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Bounds");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::show_bounding_boxes);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Bounds Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Bounds"), false);

            view_options.set_toggle(IViewer::Options::show_bounding_boxes, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Bounds"), true);
        });

    test<ViewOptions>(engine, "View Options", "Camera/Sink Checkbox Toggle",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Camera\\/Sink");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::camera_sinks);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Camera/Sink Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Camera\\/Sink"), false);

            view_options.set_toggle(IViewer::Options::camera_sinks, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Camera\\/Sink"), true);
        });

    test<ViewOptions>(engine, "View Options", "Depth Checkbox Toggle",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Depth");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::depth_enabled);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Depth Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Depth"), false);

            view_options.set_toggle(IViewer::Options::depth_enabled, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Depth"), true);
        });

    test<ViewOptions>(engine, "View Options", "Flip Checkbox Enabled",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            ctx->SetRef("View Options");
            IM_CHECK_EQ((ctx->ItemInfo("flags/Flip")->InFlags & ImGuiItemFlags_Disabled) != 0, true);
            view_options.set_flip_enabled(true);
            ctx->Yield();
            IM_CHECK_EQ((ctx->ItemInfo("flags/Flip")->InFlags & ImGuiItemFlags_Disabled) != 0, false);
        });

    test<ViewOptions>(engine, "View Options", "Flip Checkbox Hidden with Alternate Groups",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            view_options.set_use_alternate_groups(false);

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemExists("flags/Flip"), true);
            view_options.set_use_alternate_groups(true);
            ctx->Engine->InfoTasks.clear_delete();
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("flags/Flip"), false);
        });

    test<ViewOptions>(engine, "View Options", "Flip Checkbox Toggle",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            view_options.set_flip_enabled(true);

            std::optional<std::tuple<std::string, bool>> clicked;
            auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemCheck("flags/Flip");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::flip);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Flip Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            view_options.set_flip_enabled(true);

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Flip"), false);

            view_options.set_toggle(IViewer::Options::flip, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Flip"), true);
        });

    test<ViewOptions>(engine, "View Options", "Flip Flags Toggle",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            view_options.set_use_alternate_groups(true);
            view_options.set_alternate_groups({ 1, 3, 5 });

            std::unordered_map<uint32_t, bool> raised;
            auto token = view_options.on_alternate_group += [&](uint32_t group, bool state)
            {
                raised[group] = state;
            };

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip");

            IM_CHECK_EQ(raised.size(), 1);
            IM_CHECK_EQ(raised[3], true);
        });

    test<ViewOptions>(engine, "View Options", "Flip Flags Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            view_options.set_use_alternate_groups(true);
            view_options.set_alternate_groups({ 1, 3, 5 });
            view_options.set_alternate_group(3, true);

            std::unordered_map<uint32_t, bool> raised;
            auto token = view_options.on_alternate_group += [&](uint32_t group, bool state)
            {
                raised[group] = state;
            };

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip");
            IM_CHECK_EQ(raised.size(), 1);
            IM_CHECK_EQ(raised[3], false);

            ctx->Yield();
            view_options.set_alternate_group(3, true);
            raised.clear();

            ctx->SetRef("View Options");
            ctx->ItemClick("3##3_flip");
            IM_CHECK_EQ(raised.size(), 1);
            IM_CHECK_EQ(raised[3], false);
        });
}
