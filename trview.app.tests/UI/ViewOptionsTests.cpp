#include <trview.app/UI/ViewOptions.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::mocks;

TEST(ViewOptions, HighlightCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::highlight });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::highlight);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HighlightCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::highlight }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::highlight, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::highlight }) & ImGuiItemStatusFlags_Checked);
};

TEST(ViewOptions, TriggersCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::triggers });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::triggers);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, TriggersCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::triggers }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::triggers, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::triggers }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, HiddenGeometryCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::hidden_geometry });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::hidden_geometry);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, HiddenGeometryCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::hidden_geometry }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::hidden_geometry, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::hidden_geometry }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, WaterCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::water });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::water);
    ASSERT_FALSE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, WaterCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::water }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::water, false);
    imgui.render();
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::water }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, DepthCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::depth_enabled });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::depth_enabled);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, DepthCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::depth_enabled }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::depth_enabled, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::depth_enabled }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, WireframeCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::wireframe });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::wireframe);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, WireframeCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::wireframe }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::wireframe, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::wireframe }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, BoundsCheckboxToggle)
{
    ViewOptions view_options;

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::show_bounding_boxes });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::show_bounding_boxes);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, BoundsCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::show_bounding_boxes }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::show_bounding_boxes, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::show_bounding_boxes }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, FlipCheckboxToggle)
{
    ViewOptions view_options;
    view_options.set_flip_enabled(true);

    std::optional<std::tuple<std::string, bool>> clicked;
    auto token = view_options.on_toggle_changed += [&](const std::string& name, bool value)
    {
        clicked = { name, value };
    };

    tests::TestImgui imgui([&]() { view_options.render(); });
    imgui.click_element("View Options", { "flags", IViewer::Options::flip });

    ASSERT_TRUE(clicked.has_value());
    ASSERT_EQ(std::get<0>(clicked.value()), IViewer::Options::flip);
    ASSERT_TRUE(std::get<1>(clicked.value()));
}

TEST(ViewOptions, FlipCheckboxUpdated)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_FALSE(imgui.status_flags("View Options", { "flags", IViewer::Options::flip }) & ImGuiItemStatusFlags_Checked);

    view_options.set_toggle(IViewer::Options::flip, true);
    imgui.render();
    ASSERT_TRUE(imgui.status_flags("View Options", { "flags", IViewer::Options::flip }) & ImGuiItemStatusFlags_Checked);
}

TEST(ViewOptions, FlipCheckboxEnabled)
{
    ViewOptions view_options;

    tests::TestImgui imgui([&]() { view_options.render(); });
    ASSERT_TRUE(imgui.item_flags("View Options", { "flags", IViewer::Options::flip }) & ImGuiItemFlags_Disabled);

    view_options.set_flip_enabled(true);
    imgui.render();
    ASSERT_FALSE(imgui.item_flags("View Options", { "flags", IViewer::Options::flip }) & ImGuiItemFlags_Disabled);
}
/*
TEST(ViewOptions, FlipFlagsToggle)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    std::unordered_map<uint32_t, bool> raised;
    auto token = view_options.on_alternate_group += [&](uint32_t group, bool state)
    {
        raised[group] = state;
    };

    view_options.set_use_alternate_groups(true);
    view_options.set_alternate_groups({ 1, 3, 5 });

    auto group3 = window.find<ui::Button>(ViewOptions::Names::group + "3");
    ASSERT_NE(group3, nullptr);
    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOff);

    group3->clicked({});

    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOn);
    ASSERT_EQ(raised.size(), 1);
    ASSERT_EQ(raised[3], true);
}

TEST(ViewOptions, FlipFlagsUpdated)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    view_options.set_use_alternate_groups(true);
    view_options.set_alternate_groups({ 1, 3, 5 });

    auto group3 = window.find<ui::Button>(ViewOptions::Names::group + "3");
    ASSERT_NE(group3, nullptr);
    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOff);

    view_options.set_alternate_group(3, true);
    ASSERT_EQ(group3->text_background_colour(), ViewOptions::Colours::FlipOn);
}

TEST(ViewOptions, FlipCheckboxHiddenWithAlternateGroups)
{
    ui::Window window(Size(1, 1), Colour::White);
    auto view_options = ViewOptions(window, std::make_shared<JsonLoader>(std::make_shared<MockShell>()));

    auto checkbox = window.find<ui::Checkbox>(IViewer::Options::flip);
    ASSERT_TRUE(checkbox->visible());

    view_options.set_use_alternate_groups(true);
    ASSERT_FALSE(checkbox->visible(true));
}
*/