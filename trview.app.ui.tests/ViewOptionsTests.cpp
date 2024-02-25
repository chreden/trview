#include <windows.h>
#undef Yield

#include "ViewOptionsTests.h"
#include <imgui_te_context.h>
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
            ctx->Yield(1);

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
            ctx->Yield(1);

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Camera\\/Sink"), true);
        });

}
