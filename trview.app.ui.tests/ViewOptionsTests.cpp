#include "pch.h"
#include "ViewOptionsTests.h"
#include <format>

#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Windows/IViewer.h>

using namespace trview;

void register_view_options_tests(ImGuiTestEngine* engine)
{
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

    test<ViewOptions>(engine, "View Options", "Depth Value Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();
            std::optional<std::tuple<std::string, int32_t>> clicked;
            auto token = view_options.on_scalar_changed += [&](const std::string& name, int32_t value)
            {
                clicked = { name, value };
            };

            ctx->SetRef("View Options");
            ctx->ItemClick("flags/##DepthValue/+");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::depth);
            IM_CHECK_EQ(std::get<1>(clicked.value()), 1);
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

    test<ViewOptions>(engine, "View Options", "Hidden Geometry Checkbox Toggle",
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
            ctx->ItemCheck("flags/Geometry");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::geometry);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Hidden Geometry Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Geometry"), false);

            view_options.set_toggle(IViewer::Options::geometry, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Geometry"), true);
        });

    test<ViewOptions>(engine, "View Options", "Highlight Checkbox Toggle",
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
            ctx->ItemCheck("flags/Highlight");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::highlight);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Highlight Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Highlight"), false);

            view_options.set_toggle(IViewer::Options::highlight, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Highlight"), true);
        });

    test<ViewOptions>(engine, "View Options", "Items Checkbox Toggle",
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
            ctx->ItemUncheck("flags/Items");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::items);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptions>(engine, "View Options", "Items Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Items"), true);

            view_options.set_toggle(IViewer::Options::items, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Items"), false);
        });

    test<ViewOptions>(engine, "View Options", "Lighting Checkbox Toggle",
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
            ctx->ItemUncheck("flags/Lighting");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::lighting);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptions>(engine, "View Options", "Lighting Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Lighting"), true);

            view_options.set_toggle(IViewer::Options::lighting, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Lighting"), false);
        });

    test<ViewOptions>(engine, "View Options", "Notes Checkbox Toggle",
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
            ctx->ItemUncheck("flags/Notes");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::notes);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptions>(engine, "View Options", "Notes Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Notes"), true);

            view_options.set_toggle(IViewer::Options::notes, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Notes"), false);
        });

    test<ViewOptions>(engine, "View Options", "Rooms Checkbox Toggle",
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
            ctx->ItemUncheck("flags/Rooms");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::rooms);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptions>(engine, "View Options", "Rooms Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Rooms"), true);

            view_options.set_toggle(IViewer::Options::rooms, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Rooms"), false);
        });

    test<ViewOptions>(engine, "View Options", "Triggers Checkbox Toggle",
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
            ctx->ItemUncheck("flags/Triggers");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::triggers);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptions>(engine, "View Options", "Triggers Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Triggers"), true);

            view_options.set_toggle(IViewer::Options::triggers, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Triggers"), false);
        });

    test<ViewOptions>(engine, "View Options", "Water Checkbox Toggle",
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
            ctx->ItemUncheck("flags/Water");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::water);
            IM_CHECK_EQ(std::get<1>(clicked.value()), false);
        });

    test<ViewOptions>(engine, "View Options", "Water Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Water"), true);

            view_options.set_toggle(IViewer::Options::water, false);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Water"), false);
        });

    test<ViewOptions>(engine, "View Options", "Wireframe Checkbox Toggle",
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
            ctx->ItemCheck("flags/Wireframe");

            IM_CHECK_EQ(clicked.has_value(), true);
            IM_CHECK_EQ(std::get<0>(clicked.value()), IViewer::Options::wireframe);
            IM_CHECK_EQ(std::get<1>(clicked.value()), true);
        });

    test<ViewOptions>(engine, "View Options", "Wireframe Checkbox Updated",
        [](ImGuiTestContext* ctx) { ctx->GetVars<ViewOptions>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& view_options = ctx->GetVars<ViewOptions>();

            ctx->SetRef("View Options");
            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Wireframe"), false);

            view_options.set_toggle(IViewer::Options::wireframe, true);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemIsChecked("flags/Wireframe"), true);
        });
}
